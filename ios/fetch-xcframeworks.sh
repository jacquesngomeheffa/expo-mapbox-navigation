#!/bin/bash
# ─────────────────────────────────────────────────────────────────────────────
# fetch-xcframeworks.sh
# Fetches prebuilt xcframeworks for the ExpoMapboxNavigation module and
# copies them into ios/Frameworks/, which ExpoMapboxNavigation.podspec
# vendors via s.vendored_frameworks.
# ─────────────────────────────────────────────────────────────────────────────

set -e
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FRAMEWORKS_DIR="$SCRIPT_DIR/Frameworks"

# Must match the version used by @rnmapbox/maps
MAPBOX_NAV_VERSION="${MAPBOX_NAV_VERSION:-3.8.2}"

echo "🔧 Fetching prebuilt xcframeworks for Mapbox Navigation SDK v$MAPBOX_NAV_VERSION"
echo "   Output: $FRAMEWORKS_DIR"
echo ""

# ── Step 1: Clone Mapbox build-artifacts repo ───────────────────────────────
TMPDIR=$(mktemp -d)

echo "📦 Cloning mapbox-navigation-ios-build-artifacts v$MAPBOX_NAV_VERSION..."

git clone --branch "v$MAPBOX_NAV_VERSION" --depth 1 \
  https://github.com/mapbox/mapbox-navigation-ios-build-artifacts.git \
  "$TMPDIR/build-artifacts"

cd "$TMPDIR/build-artifacts"

# ── Step 1b: Remove MapboxNavigationCustomRoute (403 binary) ────────────────
echo "🩹 Removing MapboxNavigationCustomRoute from Package.swift..."

python3 <<'PYEOF'
from pathlib import Path
import re

package = Path("Package.swift")
content = package.read_text()

# Remove product
content = re.sub(
    r'\.library\(\s*name:\s*"MapboxNavigationCustomRoute".*?\),\s*\n',
    '',
    content,
    flags=re.DOTALL,
)

# Remove wrapper target
content = re.sub(
    r'\.target\(\s*name:\s*"MapboxNavigationCustomRouteWrapper".*?\),\s*\n',
    '',
    content,
    flags=re.DOTALL,
)

# Remove binaryTargets() + libraryTargets()
content = content.replace(
    'binaryTargets() + libraryTargets() + [',
    'binaryTargets() + ['
)

package.write_text(content)
PYEOF

# ── Step 1c: Fix macOS deployment target ────────────────────────────────────
echo "🩹 Updating macOS deployment target to 10.15..."

python3 <<'PYEOF'
from pathlib import Path
import re

package = Path("Package.swift")
content = package.read_text()

content = re.sub(
    r'\.macOS\(\.v10_[0-9]+\)',
    '.macOS(.v10_15)',
    content
)

package.write_text(content)
PYEOF

# ── Step 2: Resolve + download binaries ─────────────────────────────────────
echo ""
echo "⬇️  Resolving and downloading precompiled binaries..."

swift build -c release \
  --product MapboxNavigationCore \
  --product MapboxNavigationUIKit \
  --product MapboxDirections

# ── Step 3: Copy xcframeworks ───────────────────────────────────────────────
echo ""
echo "📋 Copying xcframeworks to $FRAMEWORKS_DIR..."

mkdir -p "$FRAMEWORKS_DIR"

ARTIFACTS_DIR="$TMPDIR/build-artifacts/.build/artifacts"

NEEDED_FRAMEWORKS=(
  "MapboxNavigationCore"
  "MapboxNavigationUIKit"
  "MapboxDirections"
  "_MapboxNavigationHelpers"
  "_MapboxNavigationLocalization"
  "MapboxNavigationNative"
)

for fw in "${NEEDED_FRAMEWORKS[@]}"; do
  found=$(find "$ARTIFACTS_DIR" -type d -name "${fw}.xcframework" | head -1)

  if [[ -n "$found" ]]; then
    echo "   ✅ $fw.xcframework"
    rm -rf "$FRAMEWORKS_DIR/${fw}.xcframework"
    cp -R "$found" "$FRAMEWORKS_DIR/"
  else
    echo "   ❌ $fw.xcframework not found"
  fi
done

# ── Cleanup ─────────────────────────────────────────────────────────────────
cd /
rm -rf "$TMPDIR"

echo ""
echo "✅ Done!"
echo "Frameworks copied to:"
echo "  $FRAMEWORKS_DIR"
