#!/bin/bash
# -----------------------------------------------------------------------------
# fetch-xcframeworks.sh
# Fetches prebuilt xcframeworks for the ExpoMapboxNavigation module and
# copies them into ios/Frameworks/, which ExpoMapboxNavigation.podspec
# vendors via s.vendored_frameworks.
#
# NOT committed anywhere (see .gitignore) - Mapbox's own Product Terms
# ("1.10. No Redistribution") prohibit redistributing their SDK binaries
# to third parties who haven't authenticated with their own Mapbox
# account/token, and this repository is public. This script instead runs
# on-demand, invoked automatically by ExpoMapboxNavigation.podspec's own
# s.prepare_command whenever ios/Frameworks/ is empty - meaning it runs
# inside the CONSUMING app's own `pod install`, using that app's own
# Mapbox downloadsToken. See the "Why iOS binaries aren't committed to
# this repo" section in README.md for the full reasoning.
#
# REVERTED from a prior version of this script that ALSO vendored
# MapboxMaps.xcframework itself (from mapbox-maps-ios-binary), plus a
# Podfile override forcing @rnmapbox/maps to use that same copy. That
# approach was based on a real, confirmed root cause (a Mapbox engineer's
# explanation on mapbox/mapbox-maps-ios#1669: CocoaPods-trunk MapboxMaps
# lacks BUILD_LIBRARY_FOR_DISTRIBUTION, causing a DYLD missing-symbol
# crash) - but introduced its own real, confirmed problem: the
# `:podspec =>` override broke CocoaPods' automatic "[CP] Copy
# XCFrameworks" build phase generation for that specific pod (confirmed
# via a real build log - the phase ran for every sibling Mapbox pod
# except the overridden MapboxMaps), causing a persistent "no such module
# 'MapboxMaps'" compile error that several rounds of manual xcconfig
# patching couldn't fully resolve.
#
# Reverted to plain `s.dependency 'MapboxMaps', '<version>'` in
# ExpoMapboxNavigation.podspec instead - relying on CocoaPods' own
# natural pod-name deduplication with @rnmapbox/maps' own dependency,
# matching a real, confirmed-working reference implementation
# (stefanpavlovic-tech/react-native-mapbox-navigation, commit 6ede7e1 -
# "Verified: headless xcodebuild link BUILD SUCCEEDED, zero duplicate
# symbols, all 10 Mapbox/nav frameworks embedded exactly once").
#
# HONEST CAVEAT this reversion does NOT resolve: that reference
# implementation's own verification was about the BUILD succeeding, not
# about confirmed crash-free behavior on a real device - the original
# DYLD "Symbol not found: GestureType.singleTap" launch crash this whole
# investigation started from was never confirmed fixed by this specific
# change, and going back to plain CocoaPods-trunk MapboxMaps could
# plausibly reintroduce it. This reversion's goal is narrower: get past
# the currently-blocking compile-time error first.
#
# IMPORTANT: every echo/print statement in this script is deliberately
# plain ASCII (no em-dashes, no emoji, no box-drawing characters). A real
# `pod install` failure confirmed this matters, not just style: this
# script's output is captured by CocoaPods' own Ruby code (it runs via
# s.prepare_command), and non-ASCII bytes in that captured output
# triggered a real `Encoding::CompatibilityError - incompatible character
# encodings: UTF-8 and ASCII-8BIT` failure in this exact environment.
# -----------------------------------------------------------------------------

set -e
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FRAMEWORKS_DIR="$SCRIPT_DIR/Frameworks"

# NEW: persist this script's own output to a log file, in addition to
# stdout/stderr. Needed because CocoaPods' own Executable.execute_command
# (which runs prepare_command) only DISPLAYS a command's output when
# `pod install` is run with --verbose - on a successful (exit 0) run, the
# output is captured and simply discarded, never shown, regardless of how
# much this script itself echoes. EAS Build's own "Install pods" step runs
# a plain `pod install` (no --verbose - confirmed against Expo's own iOS
# build process docs), so this is not an EAS-specific quirk - it is
# CocoaPods' own default behavior, and it is why this output has never
# been visible there, independent of the Dir.glob/exit-code bugs fixed
# earlier. `tee -a` appends (not overwrites) so a full history across
# repeated invocations is kept - each invocation is already timestamped by
# CocoaPods/EAS around it in the surrounding build log context.
LOG_FILE="$SCRIPT_DIR/.fetch-xcframeworks.log"
echo "" >> "$LOG_FILE" 2>/dev/null || true
echo "===== fetch-xcframeworks.sh run: $(date -u +%Y-%m-%dT%H:%M:%SZ) =====" >> "$LOG_FILE" 2>/dev/null || true
exec > >(tee -a "$LOG_FILE") 2>&1

# Aligned to stefanpavlovic-tech/react-native-mapbox-navigation's own
# interlocked version set (branch feat/nav-v3-spm, its own podspec
# comment): nav 3.20.1 / MapboxNavigationNative 324.20.2 / MapboxCommon
# 24.20.2 / MapboxMaps 11.20.2 - a real, confirmed-working combination,
# rather than this project's own separately-chosen 3.20.0/11.20.0/
# 324.20.0 pairing (which was only confirmed via mapbox-navigation-ios's
# own CHANGELOG.md, never actually tested end-to-end successfully).
#
# MAINTAINER: `RNMapboxMapsVersion` in the consuming app's @rnmapbox/maps
# config, AND ExpoMapboxNavigation.podspec's own
# `s.dependency 'MapboxMaps', '...'` line, must both be set to this exact
# same value - see "Upgrading the vendored iOS SDK version" in README.md.
MAPBOX_NAV_VERSION="${MAPBOX_NAV_VERSION:-3.20.1}"

echo "Fetching prebuilt xcframeworks for Mapbox Navigation SDK v$MAPBOX_NAV_VERSION"
echo "Output: $FRAMEWORKS_DIR"
echo ""

TMPDIR=$(mktemp -d)
mkdir -p "$FRAMEWORKS_DIR"

# --- Navigation-specific frameworks, from mapbox-navigation-ios-build-artifacts
echo "Cloning mapbox-navigation-ios-build-artifacts v$MAPBOX_NAV_VERSION..."
git clone --branch "v$MAPBOX_NAV_VERSION" --depth 1 \
  https://github.com/mapbox/mapbox-navigation-ios-build-artifacts.git \
  "$TMPDIR/nav-build-artifacts"

cd "$TMPDIR/nav-build-artifacts"

# --- Step 1a: Patch out MapboxNavigationCustomRoute -------------------------
# `--product` filtering on `swift build` (tried first) does NOT prevent SPM
# from resolving/validating every declared target in the manifest before
# selecting what to actually compile - it still attempts to fetch
# MapboxNavigationCustomRoute's binary even when we only ask for the other
# three products, and that specific binary 403s (gated behind separate
# account permissions we don't have and don't need). The only reliable fix
# is to remove it from the manifest entirely before building: strip its
# `.library(...)` product declaration, its `.target(...)` wrapper, and the
# `libraryTargets()` call that creates its underlying binaryTarget.
echo "Removing unused/gated MapboxNavigationCustomRoute product from Package.swift..."
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
    print("WARNING: patch made no changes - Package.swift structure may have changed upstream.")
else:
    print("Patched Package.swift")

with open("Package.swift", "w") as f:
    f.write(content)
PYEOF

echo "Resolving and downloading precompiled Navigation binaries..."
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
# FIXED: a missing framework previously only echoed "FAILED" and kept
# going - `set -e` never fired because `find ... | head -1` itself exits 0
# even when it finds nothing - so this script could report overall
# success (echoing "Done!" at the end, exit code 0) while having copied an
# incomplete set. That silent gap then only surfaced much later, as a much
# less legible Xcode compile-time "no such module" error. Now counts
# misses and aborts with exit 1, so ExpoMapboxNavigation.podspec's own
# `if ! ios/fetch-xcframeworks.sh; then exit 1; fi` wrapper (which checks
# this script's exit code) can actually catch it.
MISSING_COUNT=0
for fw in "${NEEDED_NAV_FRAMEWORKS[@]}"; do
  found=$(find "$ARTIFACTS_DIR" -iname "${fw}.xcframework" -type d | head -1)
  if [ -n "$found" ]; then
    echo "OK: $fw.xcframework"
    rm -rf "${FRAMEWORKS_DIR:?}/$fw.xcframework"
    cp -R "$found" "$FRAMEWORKS_DIR/"
  else
    echo "FAILED: $fw.xcframework not found under $ARTIFACTS_DIR"
    MISSING_COUNT=$((MISSING_COUNT + 1))
  fi
done
if [ "$MISSING_COUNT" -gt 0 ]; then
  echo ""
  echo "error: $MISSING_COUNT required xcframework(s) missing after fetch - aborting."
  exit 1
fi

# --- Deployment target patch for the vendored .swiftinterface files --------
# See README changelog for the full history of this specific fix. Kept as
# a safety net - a no-op if these particular binaries don't need it.
echo ""
echo "Patching iOS target baked into vendored .swiftinterface files (14.0 -> 15.1), if needed..."
PATCHED_COUNT=0
while IFS= read -r -d '' interface_file; do
  if grep -q -- "-ios14\.0" "$interface_file"; then
    sed -i '' 's/-ios14\.0/-ios15.1/g' "$interface_file"
    PATCHED_COUNT=$((PATCHED_COUNT + 1))
  fi
done < <(find "$FRAMEWORKS_DIR" -name "*.swiftinterface" -print0)
echo "Patched $PATCHED_COUNT .swiftinterface file(s)"

# --- Cleanup -----------------------------------------------------------------
cd /
rm -rf "$TMPDIR"

echo ""
echo "Done! xcframeworks are in $FRAMEWORKS_DIR"
echo "These are NOT committed to this repo or npm package (see .gitignore)."
echo "This script runs automatically for consumers via s.prepare_command."