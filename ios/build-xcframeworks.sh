#!/bin/bash
# ─────────────────────────────────────────────────────────────────────────────
# build-xcframeworks.sh
# Builds xcframeworks for the ExpoMapboxNavigation module.
#
# PREREQUISITES:
#   1. macOS + Xcode 16+
#   2. ~/.netrc configured with your Mapbox Downloads token:
#        machine api.mapbox.com
#        login mapbox
#        password sk.your_downloads_token
#   3. Swift Package Manager available (comes with Xcode)
#   4. Scipio, installed automatically by this script via nest
#      (https://github.com/mtj0928/nest), which downloads a prebuilt
#      release binary instead of compiling from source.
#
# This script builds MapboxNavigationCore and MapboxNavigationUIKit
# (and their dependencies) as xcframeworks and copies them into the
# ios/Frameworks/ directory, which is referenced by ExpoMapboxNavigation.podspec.
#
# Based on the approach by youssefhenna/expo-mapbox-navigation:
# https://github.com/uju777/expo-mapbox-navigation#getting-the-xcframework-files
# ─────────────────────────────────────────────────────────────────────────────

set -e
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODULE_ROOT="$(dirname "$SCRIPT_DIR")"
FRAMEWORKS_DIR="$SCRIPT_DIR/Frameworks"

# ── Config ────────────────────────────────────────────────────────────────────
# These must match what @rnmapbox/maps installs via CocoaPods in this project
# (check `pod install` log for "Installing MapboxCommon (...)" etc).
MAPBOX_NAV_VERSION="${MAPBOX_NAV_VERSION:-3.8.2}"
MAPBOX_MAPS_VERSION="11.11.0"
MAPBOX_COMMON_VERSION="24.11.0"
MAPBOX_NAV_NATIVE_VERSION="324.0.5"

echo "🔧 Building xcframeworks for Mapbox Navigation SDK v$MAPBOX_NAV_VERSION"
echo "   Output: $FRAMEWORKS_DIR"
echo ""

# ── Step 1: Clone mapbox-navigation-ios ──────────────────────────────────────
TMPDIR=$(mktemp -d)
echo "📦 Cloning mapbox-navigation-ios v$MAPBOX_NAV_VERSION..."
git clone --branch "v$MAPBOX_NAV_VERSION" --depth 1 \
  https://github.com/mapbox/mapbox-navigation-ios.git \
  "$TMPDIR/mapbox-navigation-ios"

cd "$TMPDIR/mapbox-navigation-ios"

# ── Step 2: Replace Package.swift with the modified version ──────────────────
echo "📝 Patching Package.swift..."
cp "$SCRIPT_DIR/Package.swift" Package.swift

# ── Step 3: Get the correct navNative checksum ───────────────────────────────
echo "🔍 Resolving navNative checksum (this may take a moment)..."
# Run swift build once to get the correct checksum — it will fail and print it
CHECKSUM=$(swift build -c release 2>&1 | grep -oE '"[a-f0-9]{64}"' | head -1 | tr -d '"' || true)

if [ -z "$CHECKSUM" ]; then
  echo "⚠️  Could not auto-detect checksum. Please run manually and update Package.swift."
else
  echo "   Checksum: $CHECKSUM"
  sed -i '' "s/placeholder_run_swift_build_to_get_real_checksum/$CHECKSUM/g" Package.swift
fi

# ── Step 4: Build xcframeworks with Scipio ────────────────────────────────────
echo ""
echo "🏗️  Building xcframeworks with Scipio..."
echo "   This will take 10-30 minutes on first run."
echo ""

# Install Scipio via nest, instead of building it from source ourselves.
#
# WHY: building Scipio locally with `swift build` (with or without
# --disable-sandbox) consistently produced a binary that fails immediately
# on its first real subprocess call to Xcode's internal xcbuild tool
# ("posix_spawn error: No such file or directory"), reproducibly across
# 3 different Xcode versions (16.4, 26.3) and multiple build-flag
# variations. This points to something specific about a freshly, locally
# `swift build`-compiled binary's ability to spawn subprocesses in this CI
# environment — not a version or resource problem, since the exact same
# xcbuild path works fine for `xcodebuild -version` and for every other
# process on the machine.
#
# `nest` (https://github.com/mtj0928/nest) is a package manager that
# installs Swift CLI tools from prebuilt GitHub Release artifact bundles
# rather than compiling them locally. Its own bootstrap script does this
# same trick for itself (downloads a prebuilt nest, uses it to "install"
# nest under nest's own management). Using a properly-built, officially
# published release binary for Scipio — instead of an ad-hoc local build —
# sidesteps this whole class of issue entirely, and also removes the need
# for the @retroactive patch and Scipio version pinning workarounds below,
# since we're no longer compiling Scipio's source at all.
echo "📥 Installing nest (prebuilt binary, not compiled locally)..."
curl -s https://raw.githubusercontent.com/mtj0928/nest/main/Scripts/install.sh | bash
export PATH="$HOME/.nest/bin:$PATH"

SCIPIO_VERSION="0.21.0"
echo "📥 Installing Scipio $SCIPIO_VERSION via nest (prebuilt artifact bundle)..."
nest install giginet/Scipio "$SCIPIO_VERSION"
SCIPIO_BIN="$HOME/.nest/bin/scipio"

# Build the xcframeworks
# NOTE: --support-simulators was removed. Building both device AND
# simulator slices for every target (this package's dependency graph has
# ~9 targets: MapboxNavigationCore, MapboxNavigationUIKit,
# MapboxNavigationNative, MapboxDirections, _MapboxNavigationHelpers,
# MapboxMaps, MapboxCoreMaps, MapboxCommon, Turf) roughly doubles memory
# and CPU pressure during the parallel build. On the free GitHub-hosted
# macOS runner (limited cores/RAM), this appears to exhaust resources and
# crash the underlying xcbuild build service mid-build, which then
# manifests as a confusing "posix_spawn error: No such file or directory"
# for the xcbuild binary itself on whichever target happens to be
# building next — even though the binary is actually present. Device-only
# frameworks are sufficient for real EAS/App Store builds; simulator
# support isn't needed for what this package vendors.
cd "$TMPDIR/mapbox-navigation-ios"
"$SCIPIO_BIN" create ./ -f \
  --platforms iOS \
  --only-use-versions-from-resolved-file \
  --enable-library-evolution \
  --embed-debug-symbols \
  --verbose

# ── Step 5: Copy to module ────────────────────────────────────────────────────
echo ""
echo "📋 Copying xcframeworks to $FRAMEWORKS_DIR..."
mkdir -p "$FRAMEWORKS_DIR"

BUILT_FRAMEWORKS="$TMPDIR/mapbox-navigation-ios/XCFrameworks"

# Copy the frameworks we need (others like MapboxMaps come from @rnmapbox/maps)
NEEDED_FRAMEWORKS=(
  "MapboxNavigationCore"
  "MapboxNavigationUIKit"
  "MapboxNavigationNative"
  "MapboxDirections"
  "_MapboxNavigationHelpers"
)

for fw in "${NEEDED_FRAMEWORKS[@]}"; do
  if [ -d "$BUILT_FRAMEWORKS/$fw.xcframework" ]; then
    echo "   ✅ $fw.xcframework"
    cp -R "$BUILT_FRAMEWORKS/$fw.xcframework" "$FRAMEWORKS_DIR/"
  else
    echo "   ❌ $fw.xcframework not found in $BUILT_FRAMEWORKS"
  fi
done

# ── Cleanup ────────────────────────────────────────────────────────────────────
cd /
rm -rf "$TMPDIR"

echo ""
echo "✅ Done! xcframeworks are in $FRAMEWORKS_DIR"
echo "   Commit the Frameworks/ directory and publish the package."
