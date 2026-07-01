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
MAPBOX_NAV_VERSION="3.25.0"
MAPBOX_MAPS_VERSION="11.25.0"
MAPBOX_COMMON_VERSION="24.25.0"
MAPBOX_NAV_NATIVE_VERSION="324.25.0"

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
git clone --depth 1 https://github.com/giginet/Scipio.git Scipio
cd Scipio
swift build -c release

# Build the xcframeworks
cd "$TMPDIR/mapbox-navigation-ios"
Scipio/.build/release/scipio create ./ -f \
  --platforms iOS \
  --only-use-versions-from-resolved-file \
  --enable-library-evolution \
  --support-simulators \
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
