#!/usr/bin/env bash
set -euo pipefail

# ios/fetch-xcframeworks-scipio.sh
#
# ⚠️ EXPERIMENTAL — an ALTERNATIVE to fetch-xcframeworks.sh, not a
# replacement. That script is left completely untouched and remains the
# proven, working default. Do not wire this into the normal publish flow
# until it has been validated end-to-end on a real device.
#
# WHAT THIS DOES DIFFERENTLY: fetch-xcframeworks.sh downloads Mapbox's own
# OFFICIALLY PRECOMPILED xcframeworks from mapbox-navigation-ios-build-artifacts.
# This script instead clones the mapbox-navigation-ios SOURCE repo and
# compiles MapboxNavigationCore/UIKit/etc. ourselves, from source, using
# Scipio (https://github.com/giginet/Scipio) — a real tool for turning SPM
# packages into xcframeworks.
#
# WHY THIS EXISTS: a real production launch crash — DYLD "Symbol not found:
# _$s10MapboxMaps11GestureTypeO9singleTapyA2CmFWC" (a Swift protocol
# conformance witness symbol for GestureType.singleTap) — persisted even
# after confirming, via a real crash report on a genuinely fresh build, that
# CocoaPods correctly resolved MapboxMaps to the exact required 11.14.0.
# Leading hypothesis (not proven with certainty — see README's "iOS Launch
# Crash Investigation" section): mapbox-navigation-ios-build-artifacts'
# own build pipeline and CocoaPods' separately-published MapboxMaps binary
# are two different distribution channels, and are not guaranteed to be
# ABI-identical even at a matching version *number*, particularly for
# library-evolution-enabled Swift binaries where witness table layout is
# sensitive to the exact compiler/build that produced them.
#
# This approach — build from source ourselves, against the exact same
# mapbox-maps-ios tag CocoaPods will separately resolve — is what
# youssefhenna/expo-mapbox-navigation (this package's origin project) does,
# based directly on guidance from a Mapbox engineer ("kried") on
# https://github.com/mapbox/mapbox-navigation-ios/issues/4703. This
# script's Scipio invocation below matches that guidance's documented
# command exactly.
#
# ⚠️ STATUS: WRITTEN BUT UNTESTED END-TO-END. This was authored without
# access to a network connection or a macOS/Xcode/Swift toolchain to
# actually run it — it could not be executed or verified in the
# environment that wrote it. It was built from:
#   1. kried's exact documented Scipio command (the issue above).
#   2. youssefhenna's own documented setup steps (their npm README).
#   3. A direct, line-by-line read of the REAL Package.swift at tag
#      v3.11.0 (confirmed: mapsVersion="11.14.0", navNativeVersion=
#      "324.14.0" — matching what this package already vendors and what
#      Mapbox's own v3.11.0 release notes require).
# Expect to iterate on a real Mac / GitHub Actions runner — the same way
# kried's own instructions describe deliberately running the build once to
# get a checksum error, and the same way every past version bump in this
# package's history needed real trial and error (see README changelog).
#
# ⚠️ KNOWN GAPS, not yet resolved by this script:
#   - `MapboxDirections` is NOT declared as a library `product` in
#     mapbox-navigation-ios's own Package.swift at v3.11.0 — only as an
#     internal target dependency of MapboxNavigationCore and the CLI tool.
#     Scipio only builds declared products, so without a workaround, no
#     MapboxDirections.xcframework would be produced — but this package's
#     podspec vendors and needs one. This script patches a temporary
#     product declaration into a LOCAL, uncommitted copy of Package.swift
#     before invoking Scipio (see step 2 below) — UNTESTED whether this
#     alone is sufficient; SPM target-visibility rules can have further
#     wrinkles this hasn't hit yet.
#   - `MapboxNavigationNative` is a binary SPM dependency (its own
#     .binaryTarget with a checksum, declared in mapbox-navigation-native-ios's
#     OWN Package.swift, a separate repo) — not something Scipio builds
#     from source the same way. This script does not yet locate or copy
#     it; see the explicit warning printed near the end of this script.
#   - Requires valid Mapbox DOWNLOADS:READ credentials in ~/.netrc to
#     resolve mapbox-navigation-native-ios's binary dependency during
#     `swift package resolve` / Scipio's own resolution step — same
#     requirement fetch-xcframeworks.sh already has, not a new one, but
#     worth confirming still works in this different code path.
#
# Does NOT touch, modify, or replace fetch-xcframeworks.sh in any way.

MAPBOX_NAV_VERSION="${MAPBOX_NAV_VERSION:-3.11.0}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FRAMEWORKS_DIR="$SCRIPT_DIR/Frameworks"
WORK_DIR="$(mktemp -d)"

echo "🔧 [EXPERIMENTAL] Building xcframeworks FROM SOURCE via Scipio — mapbox-navigation-ios v$MAPBOX_NAV_VERSION"
echo "   Work dir: $WORK_DIR"
echo "   Output:   $FRAMEWORKS_DIR"
echo ""

# ── 1. Clone the SOURCE repo (not mapbox-navigation-ios-build-artifacts) ───
# FIX (found from a real build log, not guessed): a shallow clone
# (--depth 1) of an ANNOTATED tag produced this warning in production —
# "refs/tags/vX.Y.Z <sha> is not a commit!" — followed immediately by
# Scipio failing with "Invalid package... the data couldn't be read
# because it isn't in the correct format" the moment it tried to read the
# manifest. Shallow-cloning annotated tags is a known problematic
# combination in git; a full clone avoids it. Slower, but this repo isn't
# huge and correctness matters more here than clone speed.
echo "📦 Cloning mapbox-navigation-ios v$MAPBOX_NAV_VERSION (source repo, full clone)..."
git clone --branch "v$MAPBOX_NAV_VERSION" \
  https://github.com/mapbox/mapbox-navigation-ios.git \
  "$WORK_DIR/mapbox-navigation-ios"

cd "$WORK_DIR/mapbox-navigation-ios"

# Fail fast with a clearer, SPM-native error message here if the manifest
# still can't be read, rather than surfacing only via Scipio's own less
# specific "Invalid package" wrapper further down.
echo "🔍 Sanity-checking the package manifest can be read..."
swift package describe --type json > /dev/null

# ── 2. Patch a MapboxDirections product declaration into a LOCAL copy ──────
# See the "KNOWN GAPS" note at the top of this file for why this exists.
# This edits the checkout inside $WORK_DIR only — never anything committed
# to this repo.
echo "🩹 Patching Package.swift: adding MapboxDirections as a declared library product..."
python3 -u - "$PWD/Package.swift" <<'PYEOF'
import sys
import re

path = sys.argv[1]
with open(path) as f:
    content = f.read()

# FIX, found from a real build log: Scipio's official docs state it
# targets Swift 6.0 and show `// swift-tools-version: 6.0` (WITH a space
# after the colon) in every example. mapbox-navigation-ios's actual
# Package.swift at v3.11.0 declares `// swift-tools-version:5.9` — no
# space. Both are valid, standard Swift syntax (Apple's own `swift
# package describe`/`swift package resolve` handle it fine — confirmed
# against a real build log, both succeeded against the unmodified file).
# But Scipio doesn't use Apple's manifest loader for this — it depends on
# its own third-party manifest parser (giginet/PackageManifestKit), which
# is a plausible, untested-but-testable candidate for why `scipio create`
# failed immediately with a generic "Invalid package... the data couldn't
# be read because it isn't in the correct format" on this exact file,
# right after standard SPM tooling read the same file with no issue.
# Normalizing to the spaced format Scipio's own docs use is a cheap,
# low-risk thing to try — NOT a confirmed fix, just the best next test.
tools_version_pattern = re.compile(r'^// swift-tools-version:(\d)', re.MULTILINE)
if tools_version_pattern.search(content):
    content = tools_version_pattern.sub(r'// swift-tools-version: \1', content, count=1)
    print("Normalized swift-tools-version line to include a space (Scipio doc format).")
else:
    print("swift-tools-version line already has a space, or pattern not found — no change made there.")

if '.library(\n            name: "MapboxDirections"' in content:
    print("MapboxDirections product already present — no patch needed.")
    with open(path, 'w') as f:
        f.write(content)
    sys.exit(0)

marker = '.library(\n            name: "MapboxNavigationCore",'
if marker not in content:
    print("ERROR: could not find insertion point in Package.swift — "
          "the file's structure may have changed since this script was "
          "written. Manual patch needed.", file=sys.stderr)
    sys.exit(1)

insertion = ('.library(\n'
             '            name: "MapboxDirections",\n'
             '            targets: ["MapboxDirections"]\n'
             '        ),\n        ' + marker.lstrip())
content = content.replace(marker, insertion, 1)
with open(path, 'w') as f:
    f.write(content)
print("Patched: MapboxDirections added as a library product.")
PYEOF

# Second check, right after patching — isolates "did my patch break the
# manifest" from "was the checkout already broken" (the first check above
# already ruled the latter out if this point is reached, since it already
# passed once on the unpatched file).
echo "🔍 Sanity-checking the patched manifest can still be read..."
swift package describe --type json > /dev/null
echo "   OK — patch did not break manifest parsing."

# Ensure Package.resolved actually exists before relying on
# --only-use-versions-from-resolved-file below — `swift package describe`
# above may or may not have generated one as a side effect depending on
# the toolchain version, so resolve explicitly rather than assume.
echo "🔒 Resolving package dependencies (writes Package.resolved)..."
swift package resolve

# ── 3. Fetch and build Scipio ───────────────────────────────────────────────
# FIX attempt #3 (previous two — full clone instead of shallow, and
# normalizing swift-tools-version's spacing — did NOT resolve the error,
# confirmed by two consecutive real build logs showing the identical
# failure). Pinning to a specific, confirmed-existing release tag (0.27.2)
# instead of blindly building whatever `main` happens to be at run time —
# unpinned "latest main" is generally bad practice for reproducibility
# regardless of whether it's the actual cause here, and removes one more
# source of variability before the next diagnostic attempt.
SCIPIO_VERSION="${SCIPIO_VERSION:-0.27.2}"
echo "📦 Cloning and building Scipio v$SCIPIO_VERSION (giginet/Scipio)..."
git clone --branch "$SCIPIO_VERSION" --depth 1 https://github.com/giginet/Scipio.git "$WORK_DIR/Scipio"
(cd "$WORK_DIR/Scipio" && swift build -c release)

SCIPIO_BIN="$WORK_DIR/Scipio/.build/release/scipio"
if [ ! -x "$SCIPIO_BIN" ]; then
  echo "❌ Scipio binary not found at $SCIPIO_BIN after build — check the Scipio build log above."
  exit 1
fi

echo "ℹ️  Scipio version check:"
"$SCIPIO_BIN" --version || echo "   (--version not supported by this Scipio release, or failed — non-fatal, continuing)"

# ── 4. Run Scipio against the (patched) mapbox-navigation-ios checkout ─────
# Exact command per kried's documented workaround on
# mapbox/mapbox-navigation-ios#4703 — also what youssefhenna/
# expo-mapbox-navigation uses for the same reason.
cd "$WORK_DIR/mapbox-navigation-ios"

# DIAGNOSTIC, added after two failed attempts at guessing the cause of
# "Invalid package... the data couldn't be read because it isn't in the
# correct format" from `scipio create` — this exact same error occurred
# twice in a row despite (1) a full (non-shallow) clone and (2) normalizing
# swift-tools-version's spacing, neither of which changed the outcome. `
# swift package describe`/`resolve` succeed against this exact file both
# times, so the problem is specific to Scipio's own manifest handling, not
# a real defect in the package. Dumping this raw information here so the
# NEXT failure (if any) comes with actual data instead of another guess.
echo "🔍 DIAGNOSTIC: swift package dump-package (first 100 lines)..."
swift package dump-package 2>&1 | head -100 || echo "   (dump-package itself failed — see output above)"
echo ""
echo "🔍 DIAGNOSTIC: full Package.swift content as fed to Scipio (first 50 lines)..."
head -50 Package.swift
echo "   [... truncated, see workflow artifact for full file if needed ...]"
echo ""

echo "🏗️  Running Scipio (building from source — this will take a while)..."
"$SCIPIO_BIN" create . -f \
  --platforms iOS \
  --only-use-versions-from-resolved-file \
  --enable-library-evolution \
  --support-simulators \
  --embed-debug-symbols \
  --verbose

# ── 5. Copy the frameworks this package actually vendors ───────────────────
# Same exact set this package has always vendored. MapboxMaps/MapboxCommon/
# MapboxCoreMaps/Turf are still intentionally NOT vendored here — they still
# come from CocoaPods via @rnmapbox/maps. This script only changes HOW
# MapboxNavigationCore/UIKit/etc. themselves get built, not the overall
# vendoring architecture (see ExpoMapboxNavigation.podspec's own comments).
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
  echo "   a real Scipio run."
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
    echo "  ⚠️  $fw.xcframework NOT FOUND at $src — check Scipio's actual output layout for this product."
  fi
done

echo ""
echo "⚠️  MapboxNavigationNative.xcframework is NOT handled by this script yet."
echo "    It's a binary SPM dependency (its own .binaryTarget + checksum, declared"
echo "    in mapbox-navigation-native-ios's OWN Package.swift, a separate repo) —"
echo "    not something Scipio compiles from source the same way as the targets"
echo "    above. It needs to be located separately (likely somewhere under SPM's"
echo "    resolved dependency checkouts) and copied in manually for now."
echo "    UNTESTED where exactly it ends up in this flow — check"
echo "    $WORK_DIR/mapbox-navigation-ios/.build/checkouts/ after a real run."
echo ""

# ── 6. Apply the same 14.0 → 15.1 deployment target patch as before ────────
# Identical reasoning to fetch-xcframeworks.sh — see that file's own
# comments for the full history. Kept as its own step here (not shared code
# with that script) since the two scripts are intentionally independent —
# see the file-level comment at the top of this file.
echo "🩹 Patching .swiftinterface deployment target references (14.0 → 15.1)..."
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
echo "✅ Done — WITH THE GAPS NOTED ABOVE STILL UNRESOLVED. Inspect"
echo "   $FRAMEWORKS_DIR carefully, and do not consider this a drop-in"
echo "   replacement for fetch-xcframeworks.sh until validated on a real"
echo "   device build."
