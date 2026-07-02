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
#   4. Scipio installed (https://github.com/giginet/Scipio):
#        swift package --disable-sandbox experimental-publish-xcbundles
#      OR install via Homebrew: brew install giginet/scipio/scipio
#
# This script builds MapboxNavigationCore and MapboxNavigationUIKit
# (and their dependencies) as xcframeworks and copies them into the
# ios/Frameworks/ directory, which is referenced by ExpoMapboxNavigation.podspec.
#
# Based on the approach by youssefhenna/expo-mapbox-navigation:
# https://github.com/uju777/expo-mapbox-navigation#getting-the-xcframework-files
# ─────────────────────────────────────────────────────────────────────────────

set -e

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

# Clone Scipio inside the navigation-ios repo
# Pinned to 0.21.0 — the exact version a Mapbox engineer confirmed works for
# building Navigation SDK v3 xcframeworks in mapbox/mapbox-navigation-ios#4703.
# (Cloning Scipio's main branch instead of a pinned tag can pull in newer
# Swift syntax, like SE-0439 trailing commas in parameter lists, that the
# CI runner's Swift compiler may not support yet — causing Scipio itself to
# fail to build with "unexpected ',' separator" errors, unrelated to this
# project's own code.)
SCIPIO_VERSION="0.21.0"
git clone --depth 1 --branch "$SCIPIO_VERSION" https://github.com/giginet/Scipio.git Scipio

# Patch Scipio's source: some `@retroactive ExpressibleByArgument`
# conformances (added defensively when Swift 6 mode was still new) are now
# rejected by newer Swift toolchains with "'retroactive' attribute does not
# apply; 'ExpressibleByArgument' is declared in this module" — the compiler
# now resolves that protocol as being in the same module as these types, so
# the marker is no longer valid. This is unrelated to this project's own
# code; it's purely a Scipio 0.21.0 / current-Swift-toolchain friction
# point. Stripping the now-invalid attribute (leaving the conformance
# itself intact) fixes it regardless of the exact Swift version in use,
# without needing to chase down a Scipio release built for this exact
# compiler.
echo "🩹 Patching Scipio: removing invalid @retroactive ExpressibleByArgument markers..."
grep -rl "@retroactive ExpressibleByArgument" Scipio/Sources | while read -r f; do
  sed -i '' 's/@retroactive ExpressibleByArgument/ExpressibleByArgument/g' "$f"
  echo "   patched: $f"
done

cd Scipio
# --disable-sandbox matches Scipio's own official release-build workflow
# (github-action-artifactbundle's example: `swift build --disable-sandbox
# -c release`). SPM's build-time sandbox can affect the resulting binary's
# runtime behavior; building without it matches how the maintainer's own
# published Scipio releases are built.
swift build --disable-sandbox -c release

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
Scipio/.build/release/scipio create ./ -f \
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
