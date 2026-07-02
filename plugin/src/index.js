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

  // ── iOS: .netrc for Mapbox downloads authentication ───────────────────────
  // NOTE: as of the vendored-xcframeworks architecture, this is no longer
  // needed at app-build time (the Mapbox Navigation binaries are pre-built
  // once via GitHub Actions and committed to this package — no live SPM
  // resolution happens during `pod install` or `xcodebuild` anymore).
  // Left in place, harmless, in case other tooling still expects it, and to
  // avoid a breaking change to the `downloadsToken` option's behavior.
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
