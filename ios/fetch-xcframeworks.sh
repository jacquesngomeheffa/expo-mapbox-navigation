#!/bin/bash
# ─────────────────────────────────────────────────────────────────────────────
# fetch-xcframeworks.sh
# Fetches prebuilt xcframeworks for the ExpoMapboxNavigation module and
# copies them into ios/Frameworks/, which ExpoMapboxNavigation.podspec
# vendors via s.vendored_frameworks.
#
# ⚠️ MAJOR CHANGE from earlier versions of this script: now ALSO vendors
# MapboxMaps.xcframework itself, from a SECOND official Mapbox repo
# (mapbox-maps-ios-binary) — not just the Navigation-specific frameworks.
# This is the actual, confirmed root cause fix for a DYLD "Symbol not
# found: GestureType.singleTap" launch crash that persisted through many
# earlier attempts (different SDK version pairings, different Swift
# source, building MapboxNavigationCore from source via Scipio instead of
# downloading it) — none of which touched the real cause.
#
# THE REAL ROOT CAUSE, confirmed directly from a Mapbox engineer on their
# own issue tracker (mapbox/mapbox-maps-ios#1669 — not this project's own
# theory):
#   "Because the CocoaPods and SwiftPM [source] versions of the framework
#    are provided by source and the project that builds it does not set
#    BUILD_LIBRARY_FOR_DISTRIBUTION to YES, some symbols are stripped from
#    the binary... In the scenario where a third party compiled framework
#    would also depend on the MapboxMaps module, the build would fail
#    (missing symbols)... a runtime error if the framework is dynamic."
# MapboxNavigationCore (whether downloaded precompiled from Mapbox, or
# built ourselves via Scipio) is exactly this "third party compiled
# framework" — it depends on MapboxMaps' FULL witness tables (library
# evolution). @rnmapbox/maps installs MapboxMaps via CocoaPods, which is
# built WITHOUT that flag — symbols missing, exactly matching this
# project's crash, regardless of which channel built MapboxNavigationCore
# itself. This was proven with real crash-report UUIDs: even a from-source
# Scipio build of MapboxNavigationCore, linked against the EXACT same
# MapboxMaps version CocoaPods installs, still crashed identically — the
# mismatch was never about MapboxNavigationCore's own build process at
# all.
#
# THE FIX: Mapbox publishes a real, official, SEPARATE binary distribution
# specifically for this — mapbox-maps-ios-binary — built WITH
# BUILD_LIBRARY_FOR_DISTRIBUTION=YES, exactly like the precompiled
# xcframeworks Mapbox already gives us for MapboxNavigationCore itself.
# Binary releases are available starting from MapboxMaps v11.20.0 —
# conveniently exactly the version Navigation SDK v3.20.0 requires (see
# MAPBOX_NAV_VERSION below), confirmed directly from
# https://github.com/mapbox/mapbox-navigation-ios/blob/main/CHANGELOG.md's
# "## 3.20.0" / "Packaging" section.
#
# WHY THIS SCRIPT STILL LOOKS SIMPLE FOR THE NAVIGATION-SPECIFIC PART (no
# Scipio, no local compilation): Mapbox officially publishes a SEPARATE
# repository, mapbox-navigation-ios-build-artifacts, whose sole purpose is
# to expose MapboxNavigationCore / MapboxNavigationUIKit / MapboxDirections
# / _MapboxNavigationHelpers / _MapboxNavigationLocalization (and their own
# transitive binary dependencies, e.g. MapboxNavigationNative) as
# precompiled .xcframework.zip downloads via the same
# api.mapbox.com/downloads/v2/... mechanism used throughout this project's
# history. We clone Mapbox's own repos AT THE MATCHING TAGS and let SwiftPM
# resolve + download the binaries using MAPBOX'S OWN Package.swift and
# checksums — nothing here is hand-transcribed.
#
# ⚠️ UNVERIFIED ASSUMPTION, flagged honestly: this script assumes
# mapbox-maps-ios-binary's own git tags directly contain (or SwiftPM-
# resolve-and-download, the same way mapbox-navigation-ios-build-artifacts
# already does) a MapboxMaps.xcframework reachable the same way. This
# mirrors the ALREADY-PROVEN pattern this script uses for the Navigation
# frameworks, but has not itself been run end-to-end — expect to iterate
# on the first real run, the same way every step of this whole
# investigation needed real trial and error.
#
# ⚠️ STILL UNRESOLVED, deliberately out of scope for this specific fix:
# @rnmapbox/maps will still install its OWN separate copy of MapboxMaps
# via CocoaPods regardless of what this script vendors — simply vendoring
# MapboxMaps.xcframework here does not by itself prevent that second copy
# from being linked too (a duplicate-symbol risk, not the missing-symbol
# crash this fix targets). Preventing that requires a Podfile-level pod
# override, implemented separately in plugin/src/index.js — see that
# file's own comments.
# ─────────────────────────────────────────────────────────────────────────────

set -e
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FRAMEWORKS_DIR="$SCRIPT_DIR/Frameworks"

# Confirmed directly from mapbox-navigation-ios's own CHANGELOG.md ("##
# 3.20.0" / "Packaging" / "MapboxNavigationCore now requires MapboxMaps
# v11.20.0" / "MapboxNavigationCore now requires MapboxNavigationNative
# v324.20.0"). 11.20.0 is also, not by our choice but by Mapbox's own
# versioning, exactly the FIRST version available via
# mapbox-maps-ios-binary's binary distribution — this pairing was chosen
# specifically because it's the oldest (most conservative, presumably
# most-tested-in-the-wild) version where the real fix (vendoring
# MapboxMaps built WITH BUILD_LIBRARY_FOR_DISTRIBUTION) is actually
# possible at all.
#
# ⚠️ MAINTAINER: `RNMapboxMapsVersion` in the consuming app's
# @rnmapbox/maps config is no longer the thing that determines which
# MapboxMaps binary actually links — this script vendors MapboxMaps
# directly now (see MAPBOX_MAPS_VERSION below and the Podfile override in
# plugin/src/index.js). Keep RNMapboxMapsVersion set to the SAME value
# anyway, so @rnmapbox/maps' own CocoaPods dependency declaration (which
# still gets resolved, even though our Podfile override replaces what it
# actually links against) doesn't produce a confusing version-mismatch
# warning during `pod install`.
MAPBOX_NAV_VERSION="${MAPBOX_NAV_VERSION:-3.20.0}"

# No longer reference-only — this now DRIVES which mapbox-maps-ios-binary
# tag gets vendored (see Step 1b below), in addition to still recording
# the pairing for MAPBOX_NAV_VERSION above. Confirmed exact for 3.20.0
# directly from mapbox-navigation-ios's own CHANGELOG.md — not assumed.
# ⚠️ MUST be updated together with MAPBOX_NAV_VERSION above, and with
# ExpoMapboxNavigation.podspec's platform/deployment-target settings if
# this new MapboxMaps version raises its own minimum — see "Upgrading the
# vendored iOS SDK version" in README.md.
MAPBOX_MAPS_VERSION="11.20.0"

echo "🔧 Fetching prebuilt xcframeworks for Mapbox Navigation SDK v$MAPBOX_NAV_VERSION + MapboxMaps v$MAPBOX_MAPS_VERSION"
echo "   Output: $FRAMEWORKS_DIR"
echo ""

TMPDIR=$(mktemp -d)
mkdir -p "$FRAMEWORKS_DIR"

# ── Step 1: Navigation-specific frameworks, from mapbox-navigation-ios-build-artifacts
echo "📦 [1/2] Cloning mapbox-navigation-ios-build-artifacts v$MAPBOX_NAV_VERSION..."
git clone --branch "v$MAPBOX_NAV_VERSION" --depth 1 \
  https://github.com/mapbox/mapbox-navigation-ios-build-artifacts.git \
  "$TMPDIR/nav-build-artifacts"

cd "$TMPDIR/nav-build-artifacts"

# ── Step 1a: Patch out MapboxNavigationCustomRoute ──────────────────────────
# `--product` filtering on `swift build` (tried first) does NOT prevent SPM
# from resolving/validating every declared target in the manifest before
# selecting what to actually compile — it still attempts to fetch
# MapboxNavigationCustomRoute's binary even when we only ask for the other
# three products, and that specific binary 403s (gated behind separate
# account permissions we don't have and don't need). The only reliable fix
# is to remove it from the manifest entirely before building: strip its
# `.library(...)` product declaration, its `.target(...)` wrapper, and the
# `libraryTargets()` call that creates its underlying binaryTarget.
echo "🩹 Removing unused/gated MapboxNavigationCustomRoute product from Package.swift..."
python3 - << 'PYEOF'
import re

with open("Package.swift") as f:
    content = f.read()

original_len = len(content)

content = re.sub(
    r'\.library\(\s*name:\s*"MapboxNavigationCustomRoute".*?\),\s*\n',
    '',
    content,
    flags=re.DOTALL,
)
content = re.sub(
    r'\.target\(\s*name:\s*"MapboxNavigationCustomRouteWrapper".*?\),\s*\n',
    '',
    content,
    flags=re.DOTALL,
)
content = content.replace('binaryTargets() + libraryTargets() + [', 'binaryTargets() + [')

# SPM defaults the (otherwise unspecified) macOS minimum to 10.13 for this
# package's own targets, but MapboxCommon/MapboxNavigationNative declare a
# macOS 10.15 minimum themselves, causing a platform-consistency error even
# though we only build for iOS.
content = content.replace(
    'platforms: [.iOS(.v14)]',
    'platforms: [.iOS(.v14), .macOS(.v10_15)]',
)

if len(content) == original_len:
    print("⚠️  WARNING: patch made no changes — Package.swift structure may have changed upstream.")
else:
    print("   patched Package.swift")

with open("Package.swift", "w") as f:
    f.write(content)
PYEOF

echo "⬇️  Resolving and downloading precompiled Navigation binaries..."
swift build -c release

ARTIFACTS_DIR="$TMPDIR/nav-build-artifacts/.build/artifacts"
NEEDED_NAV_FRAMEWORKS=(
  "MapboxNavigationCore"
  "MapboxNavigationUIKit"
  "MapboxDirections"
  "_MapboxNavigationHelpers"
  "_MapboxNavigationLocalization"
  "MapboxNavigationNative"
)
for fw in "${NEEDED_NAV_FRAMEWORKS[@]}"; do
  found=$(find "$ARTIFACTS_DIR" -iname "${fw}.xcframework" -type d | head -1)
  if [ -n "$found" ]; then
    echo "   ✅ $fw.xcframework"
    rm -rf "${FRAMEWORKS_DIR:?}/$fw.xcframework"
    cp -R "$found" "$FRAMEWORKS_DIR/"
  else
    echo "   ❌ $fw.xcframework not found under $ARTIFACTS_DIR"
  fi
done

# ── Step 2: MapboxMaps itself, from mapbox-maps-ios-binary ──────────────────
# THE ACTUAL FIX for the recurring crash — see the file-level comment above
# for the full reasoning. This is a SEPARATE Mapbox repo from the one
# above, dedicated specifically to distributing MapboxMaps as a precompiled
# binary built WITH BUILD_LIBRARY_FOR_DISTRIBUTION=YES.
echo ""
echo "📦 [2/2] Cloning mapbox-maps-ios-binary v$MAPBOX_MAPS_VERSION..."
git clone --branch "release/v$MAPBOX_MAPS_VERSION" --depth 1 \
  https://github.com/mapbox/mapbox-maps-ios-binary.git \
  "$TMPDIR/maps-binary"

cd "$TMPDIR/maps-binary"

echo "⬇️  Resolving and downloading the precompiled MapboxMaps binary..."
# ⚠️ CONFIRMED (no longer the unverified assumption flagged in the file
# header — this was hit on a real run): unlike
# mapbox-navigation-ios-build-artifacts in Step 1, mapbox-maps-ios-binary's
# MapboxMapsWrapper target does an unconditional `@_exported import
# MapboxMaps`. MapboxMaps.xcframework has NO macOS slice (Maps SDK is
# iOS-only; mapbox-maps-ios's own Package.swift comment says the macOS
# platform minimum is declared only "to enable `swift run` cli tools" for
# its dependents, not because MapboxMaps itself builds there). `swift build
# -c release` on this macOS runner targets the host (macOS) by default, so
# compiling that wrapper fails with "error: no such module 'MapboxMaps'".
#
# This is harmless for us: by the time that compile step runs, SwiftPM has
# already resolved the package graph AND fetched + unpacked every binary
# artifact we need into .build/artifacts (confirmed from a real run's
# "Fetching/Fetched binary artifact ... MapboxMaps.xcframework.zip" log
# lines, which complete before the wrapper-compile step even starts). We
# don't need a successful host build of the wrapper module — we only need
# the .xcframework it already downloaded — so tolerate this specific
# failure instead of letting `set -e` abort the whole script.
swift build -c release || echo "   ⚠️  swift build failed compiling MapboxMapsWrapper for macOS (expected — MapboxMaps has no macOS slice); continuing, since the binary artifacts were already resolved and downloaded above."

MAPS_ARTIFACTS_DIR="$TMPDIR/maps-binary/.build/artifacts"
found=$(find "$MAPS_ARTIFACTS_DIR" -iname "MapboxMaps.xcframework" -type d | head -1)
if [ -z "$found" ]; then
  # Fallback: some Mapbox binary-distribution repos commit the
  # xcframework directly into the repo tree at the release tag, rather
  # than resolving it as a SwiftPM binaryTarget artifact — check there
  # too before giving up, since this script's assumption about exactly
  # how this specific repo exposes the binary is unverified (see the
  # file-level comment above).
  found=$(find "$TMPDIR/maps-binary" -iname "MapboxMaps.xcframework" -type d | head -1)
fi
if [ -n "$found" ]; then
  echo "   ✅ MapboxMaps.xcframework"
  rm -rf "${FRAMEWORKS_DIR:?}/MapboxMaps.xcframework"
  cp -R "$found" "$FRAMEWORKS_DIR/"
else
  echo "   ❌ MapboxMaps.xcframework not found under $TMPDIR/maps-binary — this script's"
  echo "      assumption about how mapbox-maps-ios-binary exposes its binary was wrong."
  echo "      Inspect $TMPDIR/maps-binary manually (before it's cleaned up) to find the"
  echo "      real location and fix this script's search paths accordingly."
  exit 1
fi

# ── Step 3: Patch the deployment target baked into Mapbox's own binaries ────
# See earlier versions of this script / README changelog for the full
# history of this specific fix. Kept as a safety net — a no-op if these
# particular binaries don't need it.
echo ""
echo "🩹 Patching iOS target baked into vendored .swiftinterface files (14.0 → 15.1), if needed..."
PATCHED_COUNT=0
while IFS= read -r -d '' interface_file; do
  if grep -q -- "-ios14\.0" "$interface_file"; then
    sed -i '' 's/-ios14\.0/-ios15.1/g' "$interface_file"
    PATCHED_COUNT=$((PATCHED_COUNT + 1))
  fi
done < <(find "$FRAMEWORKS_DIR" -name "*.swiftinterface" -print0)
echo "   patched $PATCHED_COUNT .swiftinterface file(s)"

# ── Cleanup ───────────────────────────────────────────────────────────────
cd /
rm -rf "$TMPDIR"

echo ""
echo "✅ Done! xcframeworks are in $FRAMEWORKS_DIR"
echo "   This now includes MapboxMaps.xcframework itself — make sure"
echo "   ExpoMapboxNavigation.podspec's s.dependency list no longer declares"
echo "   'MapboxMaps' as a CocoaPods dependency (it's vendored now), and that"
echo "   the Podfile override in plugin/src/index.js is in place to stop"
echo "   @rnmapbox/maps from linking a second, separate copy."
echo "   Commit the Frameworks/ directory and publish the package."
