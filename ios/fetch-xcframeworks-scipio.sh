#!/usr/bin/env bash
set -euo pipefail

# ios/fetch-xcframeworks-scipio.sh
#
# Builds MapboxNavigationCore/UIKit/Directions/_MapboxNavigationHelpers/
# _MapboxNavigationLocalization xcframeworks FROM SOURCE via Scipio
# (https://github.com/giginet/Scipio), targeting Navigation 3.11.0 /
# MapboxMaps 11.14.0 — matching this package's own version pin in
# ExpoMapboxNavigation.podspec (`s.dependency 'MapboxMaps', '11.14.0'`).
#
# ⚠️ REWRITTEN to follow youssefhenna/expo-mapbox-navigation's actual,
# real technique (confirmed by reading their own README's "Getting the
# .xcframework files" section directly) — NOT the approach an earlier
# version of this script used (cloning the official mapbox-navigation-ios
# Package.swift and surgically patching out its test-only dependencies).
# youssefhenna does something simpler and more robust: clone the source
# repo for its Sources/ directories, then REPLACE Package.swift entirely
# with a minimal, hand-written one that only declares the products/
# dependencies/targets actually needed — never pulling in the test-only
# dependency chain (`swift-snapshot-testing`, `OHHTTPStubs`,
# `swift-argument-parser`, `_MapboxNavigationTestKit`, `TestHelper`,
# `CarPlayTestHelper`, `MapboxDirectionsCLI`) in the first place, rather
# than fetching it and then having to strip it back out. This also avoids
# needing to artificially add `MapboxDirections` as a declared library
# product to coax Scipio into building it — untested but plausible that
# Scipio builds an xcframework for every target reachable from the
# requested products regardless, not just the products themselves;
# youssefhenna's own Frameworks/ output includes MapboxDirections.xcframework
# despite never declaring it as a product either.
#
# The minimal Package.swift below was hand-written by directly reading
# the REAL, official Package.swift at tag v3.11.0 (confirmed exact
# content, not guessed) to get its precise target dependency graph for
# MapboxNavigationCore — notably that at this version (unlike
# youssefhenna's own older 3.8.0-era template, which used a manually
# pinned MapboxNavigationNative .binaryTarget with an explicit checksum),
# `mapbox-navigation-native-ios` is declared as a normal SPM package
# dependency (`.package(url: ..., exact: ...)`), not a manual binary
# target — and `MapboxNavigationCore` now also depends on
# `_MapboxNavigationLocalization`, a target that didn't exist as a
# dependency in the 3.8.0-era structure.
#
# ⚠️ STATUS: WRITTEN BUT UNTESTED END-TO-END — this environment has no
# network access or macOS/Xcode/Swift toolchain to run it. Expect real
# iteration on an actual Mac/GitHub Actions runner, the same way every
# past version bump in this package's history needed real trial and
# error (see README changelog) — this is a first attempt at reproducing
# youssefhenna's proven technique for a DIFFERENT version pairing
# (3.11.0/11.14.0) than the one they've actually verified themselves
# (3.8.0/11.11.0).

MAPBOX_NAV_VERSION="${MAPBOX_NAV_VERSION:-3.11.0}"
MAPBOX_MAPS_VERSION="11.14.0"
MAPBOX_NAV_NATIVE_VERSION="324.14.0"
SCIPIO_VERSION="${SCIPIO_VERSION:-0.27.2}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FRAMEWORKS_DIR="$SCRIPT_DIR/Frameworks"
WORK_DIR="$(mktemp -d)"

echo "🔧 Building xcframeworks FROM SOURCE via Scipio — mapbox-navigation-ios v$MAPBOX_NAV_VERSION / MapboxMaps v$MAPBOX_MAPS_VERSION"
echo "   Work dir: $WORK_DIR"
echo "   Output:   $FRAMEWORKS_DIR"
echo ""

# ── 1. Clone the SOURCE repo (for its Sources/ directories only —
#      Package.swift itself gets replaced entirely in step 2) ─────────────
echo "📦 Cloning mapbox-navigation-ios v$MAPBOX_NAV_VERSION (source repo, full clone — a shallow"
echo "   --depth 1 clone of an annotated tag caused a real 'is not a commit!' git warning"
echo "   and a downstream Scipio parse failure in an earlier version of this script)..."
git clone --branch "v$MAPBOX_NAV_VERSION" \
  https://github.com/mapbox/mapbox-navigation-ios.git \
  "$WORK_DIR/mapbox-navigation-ios"

cd "$WORK_DIR/mapbox-navigation-ios"

# ── 2. Replace Package.swift entirely with a minimal, hand-written one ────
# This is youssefhenna's actual technique — not a patch of the official
# file. Only declares what this package actually vendors. Confirmed
# against the REAL v3.11.0 Package.swift's target dependency graph
# directly (see the file-level comment above for specifics).
echo "🩹 Replacing Package.swift with a minimal, hand-written manifest..."
cat > Package.swift << PACKAGESWIFT_EOF
// swift-tools-version: 5.9
import PackageDescription

let navNativeVersion: Version = "${MAPBOX_NAV_NATIVE_VERSION}"
let mapsVersion: Version = "${MAPBOX_MAPS_VERSION}"

let package = Package(
    name: "MapboxNavigation",
    defaultLocalization: "en",
    platforms: [.iOS(.v14)],
    products: [
        .library(name: "MapboxNavigationUIKit", targets: ["MapboxNavigationUIKit"]),
        .library(name: "MapboxNavigationCore", targets: ["MapboxNavigationCore"]),
        .library(name: "_MapboxNavigationLocalization", targets: ["_MapboxNavigationLocalization"]),
        .library(name: "MapboxDirections", targets: ["MapboxDirections"]),
    ],
    dependencies: [
        .package(url: "https://github.com/mapbox/mapbox-navigation-native-ios.git", exact: navNativeVersion),
        .package(url: "https://github.com/mapbox/mapbox-maps-ios.git", exact: mapsVersion),
        .package(url: "https://github.com/mapbox/turf-swift.git", exact: "4.0.0"),
    ],
    targets: [
        .target(
            name: "MapboxNavigationUIKit",
            dependencies: ["MapboxNavigationCore"],
            exclude: ["Info.plist"],
            resources: [
                .copy("Resources/MBXInfo.plist"),
                .copy("Resources/PrivacyInfo.xcprivacy"),
            ]
        ),
        .target(name: "_MapboxNavigationHelpers"),
        .target(
            name: "_MapboxNavigationLocalization",
            dependencies: [
                "_MapboxNavigationHelpers",
            ]
        ),
        .target(
            name: "MapboxNavigationCore",
            dependencies: [
                "_MapboxNavigationLocalization",
                "_MapboxNavigationHelpers",
                .product(name: "MapboxNavigationNative", package: "mapbox-navigation-native-ios"),
                "MapboxDirections",
                .product(name: "MapboxMaps", package: "mapbox-maps-ios"),
            ],
            resources: [
                .process("Resources")
            ]
        ),
        .target(
            name: "MapboxDirections",
            dependencies: [
                .product(name: "Turf", package: "turf-swift"),
            ]
        ),
    ]
)
PACKAGESWIFT_EOF

# ── 3. Sanity-check the replaced manifest can actually be read ─────────────
# Cheap, fails fast with a clear SPM-native error if the hand-written
# manifest above doesn't match the real Sources/ directory layout at this
# tag (e.g. if a target's default source path doesn't exist, or a
# resource file referenced above isn't actually present) — rather than
# surfacing only via Scipio's own less specific error wrapper further
# down.
echo "🔍 Sanity-checking the replaced manifest can be read..."
swift package dump-package > /dev/null
echo "   OK"

echo "🔒 Resolving package dependencies (writes Package.resolved)..."
swift package resolve

# ── 4. Fetch and build Scipio ───────────────────────────────────────────────
echo "📦 Cloning and building Scipio v$SCIPIO_VERSION (giginet/Scipio)..."
git clone --branch "$SCIPIO_VERSION" --depth 1 https://github.com/giginet/Scipio.git "$WORK_DIR/Scipio"
(cd "$WORK_DIR/Scipio" && swift build -c release)

SCIPIO_BIN="$WORK_DIR/Scipio/.build/release/scipio"
if [ ! -x "$SCIPIO_BIN" ]; then
  echo "❌ Scipio binary not found at $SCIPIO_BIN after build — check the Scipio build log above."
  exit 1
fi

# ── 5. Run Scipio ────────────────────────────────────────────────────────
# Exact command per kried's documented workaround on
# mapbox/mapbox-navigation-ios#4703 — also what youssefhenna/
# expo-mapbox-navigation's own README documents using, verbatim.
#
# ⚠️ IMPORTANT, found by directly reading Xcode's own Xcode 16.3/16.4
# limitation discovered earlier in this investigation: the internal
# `xcbuild` tool Scipio depends on is missing on Xcode 16.3/16.4 on
# GitHub-hosted macOS runners (confirmed by checking every installed
# Xcode version directly on a real runner) — use Xcode 16.2 or earlier
# (set via a `maxim-lobanov/setup-xcode` step in the calling workflow,
# not this script). Also requires a matching iOS Simulator runtime
# actually installed (`xcodebuild -downloadPlatform iOS`) — a missing
# runtime caused an asset-catalog build failure inside mapbox-maps-ios's
# own source when this was first discovered.
echo "🏗️  Running Scipio (building from source — this will take a while)..."
"$SCIPIO_BIN" create . -f \
  --platforms iOS \
  --only-use-versions-from-resolved-file \
  --enable-library-evolution \
  --support-simulators \
  --embed-debug-symbols \
  --verbose

# ── 6. Copy the frameworks this package actually vendors ───────────────────
# MapboxMaps/MapboxCommon/MapboxCoreMaps/Turf are still intentionally NOT
# vendored here — they still come from CocoaPods via @rnmapbox/maps. This
# script only changes HOW MapboxNavigationCore/UIKit/etc. themselves get
# built, not the overall vendoring architecture (see
# ExpoMapboxNavigation.podspec's own comments).
NEEDED_FRAMEWORKS=(
  "MapboxNavigationCore"
  "MapboxNavigationUIKit"
  "MapboxDirections"
  "_MapboxNavigationHelpers"
  "_MapboxNavigationLocalization"
)

# UNVERIFIED: this is Scipio's documented default output directory name.
# Confirm against the real command's actual output on a real run.
SCIPIO_OUTPUT_DIR="$WORK_DIR/mapbox-navigation-ios/XCFrameworks"
if [ ! -d "$SCIPIO_OUTPUT_DIR" ]; then
  echo "❌ Expected Scipio output directory not found at $SCIPIO_OUTPUT_DIR"
  echo "   Check Scipio's actual printed output path above and update"
  echo "   SCIPIO_OUTPUT_DIR in this script accordingly — untested against"
  echo "   a real Scipio run for this specific minimal Package.swift."
  exit 1
fi

mkdir -p "$FRAMEWORKS_DIR"
for fw in "${NEEDED_FRAMEWORKS[@]}"; do
  src="$SCIPIO_OUTPUT_DIR/$fw.xcframework"
  if [ -d "$src" ]; then
    echo "  ✅ $fw.xcframework"
    rm -rf "${FRAMEWORKS_DIR:?}/$fw.xcframework"
    cp -R "$src" "$FRAMEWORKS_DIR/"
  else
    echo "  ⚠️  $fw.xcframework NOT FOUND at $src — check Scipio's actual output layout for this product/target."
  fi
done

echo ""
echo "⚠️  MapboxNavigationNative.xcframework is NOT copied by this script."
echo "    Since this minimal Package.swift declares it as a regular SPM package"
echo "    dependency (not a manual .binaryTarget like youssefhenna's older"
echo "    3.8.0-era template did), it should already resolve as its own binary"
echo "    xcframework via SPM's normal dependency resolution — likely under"
echo "    $WORK_DIR/mapbox-navigation-ios/.build/checkouts/ or wherever Scipio"
echo "    places resolved binary dependencies. UNTESTED where exactly it ends"
echo "    up — check after a real run and update this script to copy it too."
echo ""

# ── 7. Apply the 14.0 → 15.1 deployment target patch ────────────────────
# Same reasoning as this project's own earlier fetch-xcframeworks.sh
# (used when vendoring Mapbox's own precompiled mapbox-navigation-ios-
# build-artifacts binaries) — Mapbox bakes a hardcoded
# `-target arm64-apple-ios14.0` into each .swiftinterface file, which is
# below MapboxMaps' own actual minimum deployment target (15.1, confirmed
# via a real build error). UNTESTED whether Scipio's own build (with
# `--enable-library-evolution`) produces the same hardcoded value — check
# the "patched N file(s)" count below on a real run; 0 is not necessarily
# wrong if Scipio's own build already targets 15.1+ correctly.
echo "🩹 Patching .swiftinterface deployment target references (14.0 → 15.1), if needed..."
PATCHED=0
while IFS= read -r -d '' f; do
  if grep -q -- '-target arm64-apple-ios14\.0' "$f" 2>/dev/null || \
     grep -q -- '-target x86_64-apple-ios14\.0-simulator' "$f" 2>/dev/null; then
    sed -i '' \
      -e 's/-target arm64-apple-ios14\.0-simulator/-target arm64-apple-ios15.1-simulator/g' \
      -e 's/-target x86_64-apple-ios14\.0-simulator/-target x86_64-apple-ios15.1-simulator/g' \
      -e 's/-target arm64-apple-ios14\.0/-target arm64-apple-ios15.1/g' \
      "$f"
    PATCHED=$((PATCHED + 1))
  fi
done < <(find "$FRAMEWORKS_DIR" -name "*.swiftinterface" -print0)
echo "   patched $PATCHED .swiftinterface file(s)"

echo ""
echo "✅ Done — WITH THE GAPS NOTED ABOVE (MapboxNavigationNative not yet"
echo "   copied). Inspect $FRAMEWORKS_DIR carefully before publishing."
