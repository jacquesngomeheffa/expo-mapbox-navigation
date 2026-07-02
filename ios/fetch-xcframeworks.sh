#!/bin/bash
# ─────────────────────────────────────────────────────────────────────────────
# fetch-xcframeworks.sh
# Fetches prebuilt xcframeworks for the ExpoMapboxNavigation module and
# copies them into ios/Frameworks/, which ExpoMapboxNavigation.podspec
# vendors via s.vendored_frameworks.
#
# WHY THIS SCRIPT LOOKS SO SIMPLE (no Scipio, no local compilation):
# Mapbox officially publishes a SEPARATE repository,
# mapbox-navigation-ios-build-artifacts, whose sole purpose is to expose
# MapboxNavigationCore / MapboxNavigationUIKit / MapboxDirections /
# _MapboxNavigationHelpers (and their own transitive binary dependencies)
# as precompiled .xcframework.zip downloads — the exact same download
# mechanism (api.mapbox.com/downloads/v2/...) that already worked
# flawlessly, every single time, for MapboxNavigationNative/MapboxCommon/
# MapboxCoreMaps/Turf throughout this project's whole build history.
# Earlier (2024) these two targets were source-only, which is why the
# community ended up reaching for Scipio in the first place (see
# mapbox/mapbox-navigation-ios#4703) — Mapbox has since closed that gap
# themselves by publishing this dedicated artifacts repo, so we no longer
# need to build anything locally at all.
#
# We clone Mapbox's own repo AT THE MATCHING TAG and let SwiftPM resolve +
# download the binaries using MAPBOX'S OWN Package.swift and checksums —
# nothing here is hand-transcribed, so there's no risk of a copy-paste
# checksum mismatch.
# ─────────────────────────────────────────────────────────────────────────────

set -e
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FRAMEWORKS_DIR="$SCRIPT_DIR/Frameworks"

# Must match what @rnmapbox/maps installs via CocoaPods in this project
# (check `pod install` log for "Installing MapboxCommon (...)" etc). This
# exact tag was confirmed to exist in mapbox-navigation-ios-build-artifacts.
MAPBOX_NAV_VERSION="${MAPBOX_NAV_VERSION:-3.8.2}"

echo "🔧 Fetching prebuilt xcframeworks for Mapbox Navigation SDK v$MAPBOX_NAV_VERSION"
echo "   Output: $FRAMEWORKS_DIR"
echo ""

# ── Step 1: Clone Mapbox's own build-artifacts repo at the matching tag ─────
TMPDIR=$(mktemp -d)
echo "📦 Cloning mapbox-navigation-ios-build-artifacts v$MAPBOX_NAV_VERSION..."
git clone --branch "v$MAPBOX_NAV_VERSION" --depth 1 \
  https://github.com/mapbox/mapbox-navigation-ios-build-artifacts.git \
  "$TMPDIR/build-artifacts"

cd "$TMPDIR/build-artifacts"

# ── Step 2: Resolve + download the precompiled binaries ─────────────────────
# `swift build` (not just `swift package resolve`) is used deliberately:
# resolve alone only pins Package.resolved, it does not necessarily fetch
# and extract binary target .zip artifacts to disk. Building the package
# forces that download/extraction. The only "targets" that actually compile
# here are Mapbox's own tiny empty wrapper stubs (see their Package.swift:
# `path: "Sources/.empty/..."`) — this is plain `swift build`, not Scipio,
# and does not invoke Xcode's internal xcbuild the way Scipio did.
echo ""
echo "⬇️  Resolving and downloading precompiled binaries..."
# Build only the specific products we need, NOT the whole package.
# `swift build` with no args builds every declared product, including
# MapboxNavigationCustomRoute (a 4th product in Mapbox's Package.swift,
# gated behind separate account permissions — its binary download 403s
# for accounts without that specific feature enabled). We don't use
# MapboxNavigationCustomRoute at all, so we explicitly build only the
# three products that give us everything in NEEDED_FRAMEWORKS below
# (MapboxNavigationNative/MapboxCommon/MapboxCoreMaps/Turf/MapboxMaps come
# along automatically as transitive dependencies of these three).
swift build -c release \
  --product MapboxNavigationCore \
  --product MapboxNavigationUIKit \
  --product MapboxDirections

# ── Step 3: Copy the needed xcframeworks into the module ────────────────────
echo ""
echo "📋 Copying xcframeworks to $FRAMEWORKS_DIR..."
mkdir -p "$FRAMEWORKS_DIR"

ARTIFACTS_DIR="$TMPDIR/build-artifacts/.build/artifacts"

# Copy only what @rnmapbox/maps doesn't already provide via CocoaPods
# (MapboxMaps/MapboxCommon/MapboxCoreMaps/Turf stay out — vendoring a
# second copy of those would reintroduce duplicate-symbol errors, per
# mapbox/mapbox-navigation-ios#4703).
NEEDED_FRAMEWORKS=(
  "MapboxNavigationCore"
  "MapboxNavigationUIKit"
  "MapboxDirections"
  "_MapboxNavigationHelpers"
  "_MapboxNavigationLocalization"
  "MapboxNavigationNative"
)

for fw in "${NEEDED_FRAMEWORKS[@]}"; do
  found=$(find "$ARTIFACTS_DIR" -iname "${fw}.xcframework" -type d | head -1)
  if [ -n "$found" ]; then
    echo "   ✅ $fw.xcframework"
    cp -R "$found" "$FRAMEWORKS_DIR/"
  else
    echo "   ❌ $fw.xcframework not found under $ARTIFACTS_DIR"
  fi
done

# ── Cleanup ───────────────────────────────────────────────────────────────
cd /
rm -rf "$TMPDIR"

echo ""
echo "✅ Done! xcframeworks are in $FRAMEWORKS_DIR"
echo "   Commit the Frameworks/ directory and publish the package."
