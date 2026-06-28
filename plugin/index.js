const {
  withInfoPlist,
  withAppBuildGradle,
  withProjectBuildGradle,
  withAndroidManifest,
  withDangerousMod,
  createRunOncePlugin,
} = require('@expo/config-plugins');

const fs = require('fs');
const path = require('path');

// ─── Mapbox Maven block ────────────────────────────────────────────────

const MAPBOX_MAVEN_BLOCK = `
    maven {
      url 'https://api.mapbox.com/downloads/v2/releases/maven'
      authentication { basic(BasicAuthentication) }
      credentials {
        username = "mapbox"
        password = project.properties['MAPBOX_DOWNLOADS_TOKEN'] ?: System.getenv("MAPBOX_DOWNLOADS_TOKEN") ?: ""
      }
    }
`;

// ─── Inject into settings.gradle (SDK 53 FIX) ───────────────────────────

const withMapboxMavenInSettings = (config) => {
  return withDangerousMod(config, [
    'android',
    async (cfg) => {
      const settingsPath = path.join(
        cfg.modRequest.platformProjectRoot,
        'settings.gradle'
      );

      if (!fs.existsSync(settingsPath)) return cfg;

      let contents = fs.readFileSync(settingsPath, 'utf8');

      if (contents.includes('api.mapbox.com/downloads')) return cfg;

      if (contents.includes('dependencyResolutionManagement')) {
        contents = contents.replace(
          /dependencyResolutionManagement\s*\{([\s\S]*?)repositories\s*\{/,
          `dependencyResolutionManagement {$1repositories {${MAPBOX_MAVEN_BLOCK}`
        );
      } else if (contents.includes('allprojects')) {
        contents = contents.replace(
          /allprojects\s*\{([\s\S]*?)repositories\s*\{/,
          `allprojects {$1repositories {${MAPBOX_MAVEN_BLOCK}`
        );
      } else {
        contents += `
allprojects {
  repositories {${MAPBOX_MAVEN_BLOCK}
  }
}`;
      }

      fs.writeFileSync(settingsPath, contents);
      return cfg;
    },
  ]);
};

// ─── Plugin ─────────────────────────────────────────────────────────────

const withExpoMapboxNavigation = (config, options) => {
  const {
    accessToken,
    mapboxMapsVersion = '11.11.0',
    androidColorOverrides = {},
  } = options;

  if (!accessToken) {
    throw new Error('[expo-mapbox-navigation] accessToken is required');
  }

  // ── iOS ───────────────────────────────────────────────────────────────
  config = withInfoPlist(config, (cfg) => {
    cfg.modResults.MBXAccessToken = accessToken;

    cfg.modResults.NSLocationWhenInUseUsageDescription =
      cfg.modResults.NSLocationWhenInUseUsageDescription ??
      'Navigation GPS requires location access.';

    cfg.modResults.NSLocationAlwaysAndWhenInUseUsageDescription =
      cfg.modResults.NSLocationAlwaysAndWhenInUseUsageDescription ??
      'Background navigation requires location access.';

    cfg.modResults.NSMotionUsageDescription =
      cfg.modResults.NSMotionUsageDescription ??
      'Motion is used to improve navigation experience.';

    const modes = cfg.modResults.UIBackgroundModes || [];
    if (!modes.includes('audio')) modes.push('audio');
    if (!modes.includes('location')) modes.push('location');
    cfg.modResults.UIBackgroundModes = modes;

    return cfg;
  });

  // ── Android settings.gradle FIX (IMPORTANT SDK 53) ────────────────────
  config = withMapboxMavenInSettings(config);

  // ── Android Manifest ──────────────────────────────────────────────────
  config = withAndroidManifest(config, (cfg) => {
    const manifest = cfg.modResults.manifest;
    const permissions = manifest['uses-permission'] || [];

    const add = (name) => {
      if (!permissions.find((p) => p.$?.['android:name'] === name)) {
        permissions.push({ $: { 'android:name': name } });
      }
    };

    add('android.permission.ACCESS_FINE_LOCATION');
    add('android.permission.ACCESS_COARSE_LOCATION');
    add('android.permission.FOREGROUND_SERVICE');
    add('android.permission.FOREGROUND_SERVICE_LOCATION');

    manifest['uses-permission'] = permissions;

    const app = manifest.application?.[0];
    if (app) {
      const meta = app['meta-data'] || [];

      if (!meta.find((m) => m.$?.['android:name'] === 'com.mapbox.token')) {
        meta.push({
          $: {
            'android:name': 'com.mapbox.token',
            'android:value': accessToken,
          },
        });
      }

      app['meta-data'] = meta;
    }

    return cfg;
  });

  // ── App build.gradle ──────────────────────────────────────────────────
  config = withAppBuildGradle(config, (cfg) => {
    if (!cfg.modResults.contents.includes('mapbox_access_token')) {
      cfg.modResults.contents = cfg.modResults.contents.replace(
        /defaultConfig\s*\{/,
        `defaultConfig {
        resValue "string", "mapbox_access_token", "${accessToken}"`
      );
    }

    return cfg;
  });

  return config;
};

const plugin = createRunOncePlugin(
  withExpoMapboxNavigation,
  "expo-mapbox-navigation",
  "1.0.8"
);

module.exports = plugin;
module.exports.default = plugin;