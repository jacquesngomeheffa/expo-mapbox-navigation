const {
  withAppBuildGradle,
  withAndroidManifest,
  withInfoPlist,
  withDangerousMod,
} = require('@expo/config-plugins');
const fs = require('fs');
const path = require('path');

const withMapboxNavigation = (config, options = {}) => {
  const {
    accessToken,
    downloadsToken,
    mapboxMapsVersion = '11.11.0',
    mapboxNavigationVersion = null, // optional override — auto-calculated from mapboxMapsVersion if not set
    androidColorOverrides = {},
  } = options;

  if (!accessToken) {
    throw new Error(
      '[@jacques_gordon/expo-mapbox-navigation] `accessToken` is required.\n' +
      '  ["@jacques_gordon/expo-mapbox-navigation", { "accessToken": "pk.xxx" }]'
    );
  }

  if (!downloadsToken) {
    throw new Error(
      '[@jacques_gordon/expo-mapbox-navigation] `downloadsToken` (secret sk.* token) is required for iOS.\n' +
      '  ["@jacques_gordon/expo-mapbox-navigation", { "accessToken": "pk.xxx", "downloadsToken": "sk.xxx" }]'
    );
  }

  // ── Android ───────────────────────────────────────────────────────────────
  config = withAppBuildGradle(config, (mod) => {
    addAndroidConfig(mod, mapboxMapsVersion, androidColorOverrides);
    return mod;
  });

  config = withAndroidManifest(config, (mod) => {
    addAndroidPermissions(mod, accessToken);
    return mod;
  });

  // ── iOS: Info.plist — MBXAccessToken + permissions ────────────────────────
  config = withInfoPlist(config, (mod) => {
    mod.modResults.MBXAccessToken = accessToken;
    mod.modResults.NSLocationWhenInUseUsageDescription =
      mod.modResults.NSLocationWhenInUseUsageDescription ||
      'Your location is used for navigation.';
    mod.modResults.NSLocationAlwaysAndWhenInUseUsageDescription =
      mod.modResults.NSLocationAlwaysAndWhenInUseUsageDescription ||
      'Your location is used for navigation.';
    if (!mod.modResults.UIBackgroundModes) mod.modResults.UIBackgroundModes = [];
    for (const mode of ['audio', 'location']) {
      if (!mod.modResults.UIBackgroundModes.includes(mode)) {
        mod.modResults.UIBackgroundModes.push(mode);
      }
    }
    return mod;
  });

  // ── iOS: .netrc for SPM authentication ────────────────────────────────────
  // The Mapbox Navigation SDK v3 is distributed as source code via SPM only.
  // Our podspec uses spm_dependency() to declare the dependency, and SPM
  // authenticates against api.mapbox.com using ~/.netrc credentials.
  // This is the official Mapbox-documented authentication mechanism.
  config = withDangerousMod(config, [
    'ios',
    (mod) => {
      const homeDir = require('os').homedir();
      const netrcPath = path.join(homeDir, '.netrc');
      const netrcEntry = `machine api.mapbox.com\nlogin mapbox\npassword ${downloadsToken}\n`;
      let existingContent = '';
      if (fs.existsSync(netrcPath)) {
        existingContent = fs.readFileSync(netrcPath, 'utf8');
      }
      if (!existingContent.includes('machine api.mapbox.com')) {
        fs.writeFileSync(netrcPath, existingContent + netrcEntry, { mode: 0o600 });
        console.log('[@jacques_gordon/expo-mapbox-navigation] ✅ Wrote Mapbox credentials to ~/.netrc');
      }
      return mod;
    },
  ]);

  // ── iOS: Inject mapbox-navigation-ios SPM via Podfile post_install hook ──────
  //
  // This is the correct approach for adding SPM dependencies alongside
  // CocoaPods in an Expo project — copied directly from @rnmapbox/maps
  // (rnmapbox-maps.podspec, _add_spm_to_target method).
  //
  // WHY post_install hook (not pbxproj text injection):
  //   The hook runs INSIDE `pod install`, with access to the Ruby Xcodeproj
  //   object model (installer.pods_project, installer.aggregate_targets).
  //   This means:
  //     - Proper find-or-create (no duplicate symbols risk)
  //     - CocoaPods-aware (survives pod install --clean)
  //     - Works in the xcworkspace context (not just xcodeproj)
  //     - Identical to how @rnmapbox/maps itself adds SPM packages
  //
  // WHY this doesn't cause duplicate symbols (unlike spm_dependency()):
  //   spm_dependency() links the SPM framework into the Pod target AND the app
  //   target → 2 copies. This hook adds the package to ONLY the
  //   ExpoMapboxNavigation pod target + the app target, using the same
  //   XCRemoteSwiftPackageReference object → 1 copy, properly deduplicated.
  config = withDangerousMod(config, [
    'ios',
    (mod) => {
      const podfilePath = path.join(mod.modRequest.platformProjectRoot, 'Podfile');
      if (!fs.existsSync(podfilePath)) {
        console.warn('[@jacques_gordon/expo-mapbox-navigation] Podfile not found, skipping SPM hook');
        return mod;
      }

      let podfile = fs.readFileSync(podfilePath, 'utf8');

      // Guard: don't inject twice
      if (podfile.includes('# [ExpoMapboxNavigation] SPM hook')) {
        return mod;
      }

      // ── NAVIGATION VERSION STRATEGY ───────────────────────────────────────
      // CONFIRMED from the official CHANGELOG.md:
      //
      // PHASE 1 — Nav 3.1 to 3.12 (offset of +3):
      //   Navigation 3.N.x  requires  MapboxMaps 11.(N+3).x
      //   Nav 3.8.x  → Maps 11.11.x  ✅ (Maps 11.11.0 → Nav 3.8.x)
      //   Nav 3.11.x → Maps 11.14.x  ✅ (confirmed CHANGELOG)
      //   Nav 3.12.x → Maps 11.15.x  ✅ (confirmed rc.1 release note)
      //
      // PHASE 2 — Nav 3.16+ (3.13/3.14/3.15 were DELIBERATELY SKIPPED):
      //   Navigation 3.N.x  requires  MapboxMaps 11.N.x  (minors aligned)
      //   Nav 3.16.x → Maps 11.16.x  ✅
      //   Nav 3.21.5 → Maps 11.21.5  ✅ (confirmed release)
      //   Nav 3.23.1 → Maps 11.23.1  ✅ (confirmed release)
      //   Nav 3.25.0 → Maps 11.25.0  ✅ (confirmed release)
      //
      // Source: Android CHANGELOG — "3.16.x is the next version after 3.12.x.
      // For technical reasons, versions 3.13.x, 3.14.x and 3.15.x are skipped.
      // Starting from 3.16.x, the Nav SDK minor version will be aligned with
      // other Mapbox dependencies." (same policy applies to iOS)
      //
      // FORMULA:
      //   if mapsMinor <= 15: navMinor = mapsMinor - 3
      //   if mapsMinor >= 16: navMinor = mapsMinor
      //
      // EXAMPLE: mapboxMapsVersion = "11.11.0"
      //   mapsMinor = 11  (≤15, Phase 1)
      //   navMinor  = 11 - 3 = 8
      //   navMin    = "3.8.0" → SPM resolves latest 3.8.x → Maps 11.11.x ✅
      //
      // EXAMPLE: mapboxMapsVersion = "11.21.0"
      //   mapsMinor = 21  (≥16, Phase 2)
      //   navMinor  = 21
      //   navMin    = "3.21.0" → SPM resolves latest 3.21.x → Maps 11.21.x ✅
      const mapsVersion = mapboxMapsVersion || '11.11.0';
      const mapsMinor = parseInt(mapsVersion.split('.')[1], 10) || 11;
      const navMinor  = mapsMinor <= 15 ? mapsMinor - 3 : mapsMinor;
      const navMin    = mapboxNavigationVersion || `3.${navMinor}.0`;

      console.log(`[@jacques_gordon/expo-mapbox-navigation] Maps ${mapsVersion} (minor=${mapsMinor}) → Navigation ${navMin}..<3.${navMinor+1}.0`);
      console.log(`[@jacques_gordon/expo-mapbox-navigation] Phase: ${mapsMinor <= 15 ? `1 (offset -3: ${mapsMinor}-3=${navMinor})` : `2 (aligned: ${navMinor})`}`);

      // The Ruby hook — identical pattern to @rnmapbox/maps _add_spm_to_target
      const spmHook = `
# [ExpoMapboxNavigation] SPM hook — injected by @jacques_gordon/expo-mapbox-navigation
# Navigation: upToNextMinorVersion from ${navMin}
# Maps: ${mapsVersion} (minor ${mapsMinor}, ${mapsMinor <= 15 ? 'Phase 1: offset -3' : 'Phase 2: aligned'})
def _expo_mapbox_nav_add_spm(installer)
  url         = 'https://github.com/mapbox/mapbox-navigation-ios.git'
  requirement = { kind: 'upToNextMinorVersion', minimumVersion: '${navMin}' }
  products    = ['MapboxNavigationCore', 'MapboxNavigationUIKit']

  pkg_class = Xcodeproj::Project::Object::XCRemoteSwiftPackageReference
  ref_class = Xcodeproj::Project::Object::XCSwiftPackageProductDependency

  # ── Step 1: Add to pods_project (where ExpoMapboxNavigation target lives) ──
  pods_project = installer.pods_project

  pkg = pods_project.root_object.package_references.find { |p|
    p.class == pkg_class && p.repositoryURL == url
  }
  unless pkg
    pkg = pods_project.new(pkg_class)
    pkg.repositoryURL = url
    pkg.requirement   = requirement
    pods_project.root_object.package_references << pkg
    puts '[ExpoMapboxNavigation] Added mapbox-navigation-ios to pods_project'
  end

  # ── FIX: Stronger target lookup with fallback ──────────────────────────────
  # CocoaPods normally names the target exactly 'ExpoMapboxNavigation'.
  # Deduplication suffixes (e.g. 'ExpoMapboxNavigation-abc123') only happen
  # when the same pod is included in multiple targets with different specs,
  # which is not our case. We still add an include? fallback to be safe.
  expo_target = pods_project.targets.find { |t| t.name == 'ExpoMapboxNavigation' }
  expo_target ||= pods_project.targets.find { |t| t.name.include?('ExpoMapboxNavigation') }
  if expo_target
    puts "[ExpoMapboxNavigation] Found target: #{expo_target.name}"
    products.each do |product_name|
      ref = expo_target.package_product_dependencies.find { |r|
        r.class == ref_class && r.package == pkg && r.product_name == product_name
      }
      unless ref
        ref = pods_project.new(ref_class)
        ref.package      = pkg
        ref.product_name = product_name
        expo_target.package_product_dependencies << ref
        puts "[ExpoMapboxNavigation] Linked #{product_name} -> #{expo_target.name}"
      end
    end
  else
    # Debug: print all available targets so we can fix the name if needed
    puts '[ExpoMapboxNavigation] WARNING: ExpoMapboxNavigation target not found!'
    puts '[ExpoMapboxNavigation] Available targets:'
    pods_project.targets.each { |t| puts "  - #{t.name}" }
  end
  pods_project.save

  # ── NOTE: We do NOT add products to the user app target ────────────────────
  # Adding MapboxNavigationCore/UIKit directly to the app target causes
  # "Multiple commands produce" errors for MapboxCommon, MapboxCoreMaps, Turf
  # because @rnmapbox/maps already embeds them via CocoaPods/SPM.
  # The ExpoMapboxNavigation pod target is sufficient: when CocoaPods links
  # the pod into the app, SPM resolves the transitive dependencies correctly
  # without re-embedding them.
end
`;

      // Find the last post_install block and add our call inside it,
      // or add a new post_install block if none exists.
      if (podfile.includes('post_install do |installer|')) {
        // Add our helper def before the first post_install
        // and our call inside the existing post_install
        podfile = spmHook + podfile.replace(
          'post_install do |installer|',
          'post_install do |installer|\n  _expo_mapbox_nav_add_spm(installer)'
        );
      } else {
        // No post_install block — add both the helper and a new block
        podfile = podfile + spmHook + `
post_install do |installer|
  _expo_mapbox_nav_add_spm(installer)
end
`;
      }

      fs.writeFileSync(podfilePath, podfile, 'utf8');
      console.log('[@jacques_gordon/expo-mapbox-navigation] ✅ Injected mapbox-navigation-ios SPM hook into Podfile');
      return mod;
    },
  ]);

  return config;
};

// ── Android helpers ───────────────────────────────────────────────────────────

function addAndroidConfig(mod, mapboxMapsVersion, androidColorOverrides) {
  if (!mod.modResults.contents.includes('abiFilters')) {
    mod.modResults.contents = mod.modResults.contents.replace(
      /defaultConfig {([\s\S]*?)}/,
      `defaultConfig {\n          ndk {\n              abiFilters "arm64-v8a", "x86_64"\n          }\n          $1\n      }`
    );
  }

  const workVersion = '2.8.0';
  mod.modResults.contents = mod.modResults.contents.replace(
    /implementation ['"]androidx.work:work-runtime-ktx:[\d.]+['"]/,
    ''
  );
  if (!mod.modResults.contents.includes('work-runtime')) {
    mod.modResults.contents += `
    dependencies {
        implementation 'androidx.work:work-runtime:${workVersion}'
        implementation 'androidx.work:work-runtime-ktx:${workVersion}'
    }
  `;
  }

  if (!mod.modResults.contents.includes('dependencySubstitution')) {
    const MAPS_VER = mapboxMapsVersion || '11.11.0';
    const COMMON_VER = '24.11.3';
    mod.modResults.contents += `
      configurations.all {
        resolutionStrategy {
          dependencySubstitution {
            substitute module('com.mapbox.maps:android') using module('com.mapbox.maps:android-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.maps:android-core') using module('com.mapbox.maps:android-core-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.maps:base') using module('com.mapbox.maps:base-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.common:common') using module('com.mapbox.common:common-ndk27:${COMMON_VER}')
            substitute module('com.mapbox.module:maps-telemetry') using module('com.mapbox.module:maps-telemetry-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-attribution') using module('com.mapbox.plugin:maps-attribution-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-scalebar') using module('com.mapbox.plugin:maps-scalebar-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-gestures') using module('com.mapbox.plugin:maps-gestures-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-logo') using module('com.mapbox.plugin:maps-logo-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-compass') using module('com.mapbox.plugin:maps-compass-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-lifecycle') using module('com.mapbox.plugin:maps-lifecycle-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-animation') using module('com.mapbox.plugin:maps-animation-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-overlay') using module('com.mapbox.plugin:maps-overlay-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-annotation') using module('com.mapbox.plugin:maps-annotation-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-locationcomponent') using module('com.mapbox.plugin:maps-locationcomponent-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.plugin:maps-viewport') using module('com.mapbox.plugin:maps-viewport-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.extension:maps-localization') using module('com.mapbox.extension:maps-localization-ndk27:${MAPS_VER}')
            substitute module('com.mapbox.extension:maps-style') using module('com.mapbox.extension:maps-style-ndk27:${MAPS_VER}')
          }
        }
      }
    `;
  }

  // Android color overrides for Mapbox resource colors (route line, banner, etc.)
  if (Object.keys(androidColorOverrides).length > 0) {
    const resDir = path.join(
      mod.modRequest?.platformProjectRoot || '',
      'app', 'src', 'main', 'res', 'values'
    );
    try {
      fs.mkdirSync(resDir, { recursive: true });
      const colorEntries = Object.entries(androidColorOverrides)
        .map(([name, value]) => `    <color name="${name}">${value}</color>`)
        .join('\n');
      const xmlContent = `<?xml version="1.0" encoding="utf-8"?>\n<resources>\n${colorEntries}\n</resources>\n`;
      fs.writeFileSync(path.join(resDir, 'mapbox_color_overrides.xml'), xmlContent);
    } catch (e) {
      // Ignore — resDir may not exist at plugin resolution time
    }
  }
}

function addAndroidPermissions(mod, accessToken) {
  const manifest = mod.modResults.manifest;
  const application = manifest.application[0];

  if (!application['meta-data']) application['meta-data'] = [];
  const existingToken = application['meta-data'].find(
    (item) => item['$']['android:name'] === 'com.mapbox.token'
  );
  if (!existingToken) {
    application['meta-data'].push({
      $: { 'android:name': 'com.mapbox.token', 'android:value': accessToken },
    });
  }

  const requiredPermissions = [
    'android.permission.ACCESS_FINE_LOCATION',
    'android.permission.ACCESS_COARSE_LOCATION',
    'android.permission.FOREGROUND_SERVICE',
    'android.permission.FOREGROUND_SERVICE_LOCATION',
    'android.permission.POST_NOTIFICATIONS',
  ];
  if (!manifest['uses-permission']) manifest['uses-permission'] = [];
  for (const perm of requiredPermissions) {
    if (!manifest['uses-permission'].find((p) => p['$']['android:name'] === perm)) {
      manifest['uses-permission'].push({ $: { 'android:name': perm } });
    }
  }

  if (!manifest.service) manifest.service = [];
  const svcName = 'com.mapbox.navigation.core.trip.service.NavigationNotificationService';
  if (!manifest.service.find((s) => s['$']['android:name'] === svcName)) {
    manifest.service.push({
      $: {
        'android:name': svcName,
        'android:foregroundServiceType': 'location',
        'android:exported': 'false',
      },
    });
  }
}

module.exports = withMapboxNavigation;
