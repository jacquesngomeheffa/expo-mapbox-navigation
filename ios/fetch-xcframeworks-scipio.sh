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
# Confirmed via a real `pod install` log seen earlier in this
# investigation (MapboxMaps 11.14.0 paired with "Installing MapboxCommon
# (24.14.0)") — not guessed.
MAPBOX_COMMON_VERSION="24.14.0"
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

# ── 2. Replace Package.swift entirely with youssefhenna's own template ────
# CORRECTED from an earlier version of this script, which hand-wrote a
# manifest based on reading the REAL, OFFICIAL Package.swift at v3.11.0
# directly — that file declares `mapbox-navigation-native-ios` as a
# regular `.package()` dependency. A real build confirmed this hits a
# known, externally-documented SwiftPM limitation, unrelated to anything
# specific to this project: transitive dependencies of binary targets
# are not reliably resolved when the CONSUMING target itself becomes a
# binary product (see https://github.com/tuist/tuist/issues/8056 for the
# exact same symptom — "linking/runtime errors... missing symbols for
# types/functions defined in the transitive dependency of the
# xcframework" — on a completely different tool, confirming this isn't
# Scipio-specific or specific to any mistake in an earlier version of
# this script).
#
# youssefhenna/expo-mapbox-navigation's own template (confirmed working
# for their own 3.8.0/11.11.0 build) sidesteps this entirely by declaring
# MapboxNavigationNative as a manually-pinned `.binaryTarget` with an
# explicit checksum — a structurally different dependency declaration
# that doesn't hit the same SPM bug. This script now follows that exact
# structure (per youssefhenna's own README instructions: "update the
# versions according to the cloned branch" — reusing their template
# as-is, not re-deriving a new structure from the official file at this
# different tag), only swapping in the version numbers for this
# script's target pairing (3.11.0/11.14.0/324.14.0), not the official
# file's newer dependency-declaration style.
echo "🩹 Replacing Package.swift with youssefhenna's own template structure..."

# youssefhenna's own README documents this checksum as something you
# only discover by letting a build fail once ("Checksum for
# navNativeChecksum will be incorrect. Run swift build -c release which
# will fail and give you the correct checksum to update with. After
# updating, proceed with steps, do not re-run.") — automated here as a
# two-pass process instead of a manual one, so this script doesn't
# require interactive babysitting on a CI runner.
PLACEHOLDER_CHECKSUM="0000000000000000000000000000000000000000000000000000000000000000000000"

write_package_swift() {
  local checksum="$1"
  cat > Package.swift << PACKAGESWIFT_EOF
// swift-tools-version: 5.9
import PackageDescription

let (navNativeVersion, navNativeChecksum) = ("${MAPBOX_NAV_NATIVE_VERSION}", "${checksum}")
let mapsVersion: Version = "${MAPBOX_MAPS_VERSION}"
let commonVersion: Version = "${MAPBOX_COMMON_VERSION}"
let mapboxApiDownloads = "https://api.mapbox.com/downloads/v2"

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
        .package(url: "https://github.com/mapbox/mapbox-maps-ios.git", exact: mapsVersion),
        .package(url: "https://github.com/mapbox/mapbox-common-ios.git", exact: commonVersion),
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
                .product(name: "MapboxCommon", package: "mapbox-common-ios"),
                "MapboxNavigationNative",
                "MapboxDirections",
                "_MapboxNavigationLocalization",
                "_MapboxNavigationHelpers",
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
        navNativeBinaryTarget(
            name: "MapboxNavigationNative",
            version: navNativeVersion,
            checksum: navNativeChecksum
        ),
    ]
)

private func navNativeBinaryTarget(name: String, version: String, checksum: String) -> Target {
    let url = "\(mapboxApiDownloads)/dash-native/releases/ios/packages/\(version)/MapboxNavigationNative.xcframework.zip"
    return .binaryTarget(name: name, url: url, checksum: checksum)
}
PACKAGESWIFT_EOF
}

write_package_swift "$PLACEHOLDER_CHECKSUM"

# ── 3. Discover the real checksum (youssefhenna's manual step, automated) ──
# The placeholder checksum above is deliberately wrong. `swift package
# resolve` will fail, but Swift's own error message includes the correct
# checksum it actually computed from the downloaded artifact — extract
# it and rewrite Package.swift with the real value, matching
# youssefhenna's documented manual process exactly, just without needing
# a human to copy-paste it between two runs.
echo "🔍 Resolving once with a placeholder checksum to discover the real one..."
RESOLVE_OUTPUT="$(swift package resolve 2>&1)" || true
echo "$RESOLVE_OUTPUT"

REAL_CHECKSUM="$(echo "$RESOLVE_OUTPUT" | grep -oE 'checksum of downloaded artifact of binary target .MapboxNavigationNative. \([a-f0-9]+\)' | grep -oE '[a-f0-9]{16,}' | head -1)"

if [ -z "$REAL_CHECKSUM" ]; then
  echo "❌ Could not extract the real checksum from swift package resolve's output above."
  echo "   Swift's error message format may have changed — inspect the output manually"
  echo "   and update this script's extraction regex if needed."
  exit 1
fi

echo "✅ Discovered real checksum: $REAL_CHECKSUM"
echo "🩹 Rewriting Package.swift with the real checksum..."
write_package_swift "$REAL_CHECKSUM"

# ── 4. Sanity-check the corrected manifest can actually be read ───────────
echo "🔍 Sanity-checking the corrected manifest can be read..."
swift package dump-package > /dev/null
echo "   OK"

echo "🔒 Resolving package dependencies for real (writes Package.resolved)..."
swift package resolve

# ── 5. Fetch and build Scipio ───────────────────────────────────────────────
echo "📦 Cloning and building Scipio v$SCIPIO_VERSION (giginet/Scipio)..."
git clone --branch "$SCIPIO_VERSION" --depth 1 https://github.com/giginet/Scipio.git "$WORK_DIR/Scipio"
(cd "$WORK_DIR/Scipio" && swift build -c release)

SCIPIO_BIN="$WORK_DIR/Scipio/.build/release/scipio"
if [ ! -x "$SCIPIO_BIN" ]; then
  echo "❌ Scipio binary not found at $SCIPIO_BIN after build — check the Scipio build log above."
  exit 1
fi

# ── 6. Run Scipio ────────────────────────────────────────────────────────
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

# ── 7. Copy the frameworks this package actually vendors ───────────────────
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
  "MapboxNavigationNative"
)

# UNVERIFIED: this is Scipio's documented default output directory name.
# Confirm against the real command's actual output on a real run.
SCIPIO_OUTPUT_DIR="$WORK_DIR/mapbox-navigation-ios/XCFrameworks"
if [ ! -d "$SCIPIO_OUTPUT_DIR" ]; then
  echo "❌ Expected Scipio output directory not found at $SCIPIO_OUTPUT_DIR"
  echo "   Check Scipio's actual printed output path above and update"
  echo "   SCIPIO_OUTPUT_DIR in this script accordingly — untested against"
  echo "   a real Scipio run for this specific manifest."
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

# ── 8. Apply the 14.0 → 15.1 deployment target patch ────────────────────
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
