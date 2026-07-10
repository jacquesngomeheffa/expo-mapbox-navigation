#!/bin/bash
# -----------------------------------------------------------------------------
# patch-mapbox-library-evolution.sh
#
# WHY THIS EXISTS: MapboxNavigationCore.xcframework (vendored by this package,
# see ios/fetch-xcframeworks.sh) is a precompiled binary built by Mapbox WITH
# Swift's library evolution mode (BUILD_LIBRARY_FOR_DISTRIBUTION=YES) - the
# standard requirement for anything distributed as a binary framework. It
# depends on MapboxMaps (and MapboxMaps' own siblings: MapboxCommon,
# MapboxCoreMaps, Turf), which arrive in the consuming app via CocoaPods
# trunk - compiled WITHOUT library evolution by default. This mismatch is a
# confirmed, real mechanism (a Mapbox engineer's own explanation, verbatim,
# on mapbox/mapbox-maps-ios#1669): a precompiled framework depending on a
# CocoaPods-compiled module without library evolution can produce a *runtime*
# "Symbol not found" dyld crash (not a compile-time error) - matching exactly
# the `Symbol not found: _$s10MapboxMaps11GestureTypeO9singleTapyA2CmFWC`
# crash this was written to address.
#
# WHAT THIS DOES: after `pod install` has finished generating each target's
# .xcconfig file(s), this script appends `BUILD_LIBRARY_FOR_DISTRIBUTION = YES`
# to every .xcconfig file belonging to the Mapbox pods that MapboxNavigationCore
# actually links against - MapboxMaps itself, plus its own sibling dependencies
# (MapboxCommon, MapboxCoreMaps, Turf), since setting this flag on only ONE of
# several tightly-coupled binaries can itself produce a *different* missing-
# symbol crash (a real, confirmed CocoaPods issue - #11153).
#
# WHAT THIS DOES NOT DO: this does not touch how MapboxMaps is FETCHED or
# RESOLVED (no vendoring, no Podfile pod-source override, no second
# post_install block). This project's own history (see README changelog,
# 3.3.0-3.3.4) already tried vendoring MapboxMaps itself from Mapbox's own
# BUILD_LIBRARY_FOR_DISTRIBUTION-enabled binary distribution
# (mapbox-maps-ios-binary) via a Podfile pod-source override - it never got
# past a DIFFERENT real compile-time conflict with @rnmapbox/maps' own build
# logic ("no such module 'MapboxMaps'" in @rnmapbox/maps' own Swift source,
# confirmed on a real device archive build) and was reverted. This script
# deliberately does something much narrower instead: MapboxMaps keeps coming
# from CocoaPods trunk exactly as today (so @rnmapbox/maps' own resolution
# logic is completely undisturbed) - only ONE build setting on the resulting
# generated .xcconfig files is changed, after the fact, via a plain script.
#
# WHY NOT A POD FILE `post_install` HOOK: CocoaPods hard-rejects a second
# top-level `post_install do |installer| ... end` block in a single Podfile
# (confirmed: "Specifying multiple post_install hooks is unsupported") - and
# @rnmapbox/maps' own Expo config plugin already injects one. This project's
# own history (changelog 3.3.2/3.3.3) already hit exactly that hard failure
# once, from a previous, different attempt to patch a build setting this way.
# A separate script sidesteps the constraint entirely - and is easier to
# test/verify in isolation than Ruby code embedded in a generated Podfile.
#
# ALSO CONFIRMED (a separate, real CocoaPods issue - #8972): setting
# `config.build_settings[...]` inside a `post_install` hook does not always
# get reflected in the final generated .xcconfig file - CocoaPods can
# regenerate it afterward with default values, silently. Patching the
# .xcconfig file directly, after `pod install` has fully finished, avoids
# this class of problem outright: there is no later regeneration step to
# race against.
#
# WHEN TO RUN THIS: after `pod install` completes, before the actual Xcode
# build/archive step (which is what actually invokes swiftc and would compile
# MapboxMaps' source using whatever is in its .xcconfig at that point). On
# EAS Build, the documented `eas-build-post-install` npm hook fires at
# exactly this point (confirmed against Expo's own build lifecycle hooks
# docs: "for iOS, after both yarn install and pod install have completed").
# For local `expo run:ios` / manual `pod install`, run this script by hand
# (or wire your own local post-install step) before opening/building the
# Xcode workspace - this project's own Podfile cannot invoke it automatically
# for the reasons above.
#
# HONEST CAVEAT: this addresses a plausible, well-documented root cause for
# the DYLD "Symbol not found: GestureType.singleTap" crash - it is not a
# guaranteed fix for that exact crash. Confirm by rebuilding and checking the
# same crash no longer reproduces on a real device.
# -----------------------------------------------------------------------------

set -e
set -o pipefail

# Every ".xcconfig" produced by CocoaPods for these targets gets patched.
# These four are MapboxNavigationCore's own linked dependencies (see this
# package's ExpoMapboxNavigation.podspec comments) - not an arbitrary list.
MAPBOX_TARGETS=(
  "MapboxMaps"
  "MapboxCommon"
  "MapboxCoreMaps"
  "Turf"
)

SETTING_LINE='BUILD_LIBRARY_FOR_DISTRIBUTION = YES'

# Locate the ios/Pods directory relative to wherever this script is invoked
# from. Accepts an optional override as $1 for testing / non-standard
# layouts, so this can be exercised without a real Xcode/CocoaPods install.
PODS_DIR="${1:-ios/Pods}"

if [ ! -d "$PODS_DIR" ]; then
  echo "error: [patch-mapbox-library-evolution] Pods directory not found at '$PODS_DIR'."
  echo "       Run this AFTER 'pod install' has completed, from your project root"
  echo "       (or pass the Pods directory path as the first argument)."
  exit 1
fi

PATCHED_COUNT=0
MISSING_TARGETS=0

for target in "${MAPBOX_TARGETS[@]}"; do
  TARGET_SUPPORT_DIR="$PODS_DIR/Target Support Files/$target"
  if [ ! -d "$TARGET_SUPPORT_DIR" ]; then
    echo "warning: [patch-mapbox-library-evolution] no Target Support Files directory for '$target' - skipping (not installed, or CocoaPods changed its layout)."
    MISSING_TARGETS=$((MISSING_TARGETS + 1))
    continue
  fi

  found_any_xcconfig=0
  while IFS= read -r -d '' xcconfig_file; do
    found_any_xcconfig=1
    if grep -q "BUILD_LIBRARY_FOR_DISTRIBUTION" "$xcconfig_file"; then
      echo "skip: $xcconfig_file already sets BUILD_LIBRARY_FOR_DISTRIBUTION - not touching it."
      continue
    fi
    printf '\n%s\n' "$SETTING_LINE" >> "$xcconfig_file"
    echo "patched: $xcconfig_file"
    PATCHED_COUNT=$((PATCHED_COUNT + 1))
  done < <(find "$TARGET_SUPPORT_DIR" -name "*.xcconfig" -print0)

  if [ "$found_any_xcconfig" -eq 0 ]; then
    echo "warning: [patch-mapbox-library-evolution] '$target' has a Target Support Files directory but no .xcconfig files inside it - skipping."
    MISSING_TARGETS=$((MISSING_TARGETS + 1))
  fi
done

echo ""
echo "Patched $PATCHED_COUNT .xcconfig file(s) with BUILD_LIBRARY_FOR_DISTRIBUTION = YES."
if [ "$MISSING_TARGETS" -gt 0 ]; then
  echo "warning: $MISSING_TARGETS of ${#MAPBOX_TARGETS[@]} expected Mapbox target(s) were not found - see warnings above."
  echo "         This is not necessarily an error (e.g. Turf may not be a direct pod in every consumer app's Podfile.lock)."
fi
