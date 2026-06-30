const { withAppBuildGradle, withProjectBuildGradle, withAndroidManifest, withInfoPlist, withDangerousMod } = require('@expo/config-plugins');
const fs = require('fs');
const path = require('path');

const NDK_VERSION = '27.0.12077973';
const MAPBOX_MAPS_MIN_VERSION = '11.11.0';

const withMapboxNavigation = (config, options = {}) => {
  const {
    accessToken,
    downloadsToken,
    mapboxMapsVersion = MAPBOX_MAPS_MIN_VERSION,
    androidColorOverrides = {},
  } = options;

  if (!accessToken) {
    throw new Error(
      '[@jacques_gordon/expo-mapbox-navigation] `accessToken` is required in the plugin config.\n' +
      'Add it to your app.json plugins array:\n' +
      '  ["@jacques_gordon/expo-mapbox-navigation", { "accessToken": "pk.your_token" }]'
    );
  }

  // downloadsToken (a secret sk.* token with the Downloads:Read scope) is
  // required on iOS to authenticate Swift Package Manager's fetch of the
  // Mapbox Navigation SDK package — without it, `pod install`/`expo
  // prebuild` will fail to resolve the SPM dependency declared in
  // ExpoMapboxNavigation.podspec.
  if (!downloadsToken) {
    throw new Error(
      '[@jacques_gordon/expo-mapbox-navigation] `downloadsToken` is required for iOS builds.\n' +
      'This must be a SECRET Mapbox token (starts with "sk.") with the "Downloads:Read" scope,\n' +
      'used to authenticate Swift Package Manager when fetching the Mapbox Navigation SDK.\n' +
      'Add it to your app.json plugins array:\n' +
      '  ["@jacques_gordon/expo-mapbox-navigation", { "accessToken": "pk.xxx", "downloadsToken": "sk.xxx" }]'
    );
  }

  const [major, minor] = mapboxMapsVersion.split('.').map(Number);
  if (major < 11 || (major === 11 && minor < 11)) {
    throw new Error(
      `[@jacques_gordon/expo-mapbox-navigation] mapboxMapsVersion must be >= 11.11.0.\n` +
      `Provided: ${mapboxMapsVersion}`
    );
  }

  // ── Android: project-level build.gradle ─────────────────────────────────
  config = withProjectBuildGradle(config, (mod) => {
    let contents = mod.modResults.contents;

    if (!contents.includes('api.mapbox.com/downloads/v2/releases/maven')) {
      const mavenBlock = `
        maven {
            url 'https://api.mapbox.com/downloads/v2/releases/maven'
            authentication { basic(BasicAuthentication) }
            credentials {
                username = 'mapbox'
                password = project.hasProperty('MAPBOX_DOWNLOADS_TOKEN')
                    ? project.property('MAPBOX_DOWNLOADS_TOKEN')
                    : System.getenv('MAPBOX_DOWNLOADS_TOKEN') ?: ""
            }
        }`;
      if (contents.includes('allprojects') && contents.includes('repositories')) {
        contents = contents.replace(
          /allprojects\s*\{[\s\S]*?repositories\s*\{/,
          (match) => match + mavenBlock
        );
      }
    }

    mod.modResults.contents = contents;
    return mod;
  });

  // ── Android: app-level build.gradle ─────────────────────────────────────
  config = withAppBuildGradle(config, (mod) => {
    let contents = mod.modResults.contents;

    if (!contents.includes('ndkVersion')) {
      contents = contents.replace(
        /android\s*\{/,
        `android {\n    ndkVersion "${NDK_VERSION}"`
      );
    } else if (!contents.includes(NDK_VERSION)) {
      contents = contents.replace(
        /ndkVersion\s+["'][^"']*["']/,
        `ndkVersion "${NDK_VERSION}"`
      );
    }

    if (!contents.includes('useLegacyPackaging')) {
      contents = contents.replace(
        /android\s*\{/,
        `android {\n    packagingOptions {\n        jniLibs {\n            useLegacyPackaging = false\n        }\n    }`
      );
    }

    const resolutionBlock = `
configurations.all {
    resolutionStrategy {
        force "com.mapbox.maps:android:${mapboxMapsVersion}"
        force "com.mapbox.maps:android-ndk27:${mapboxMapsVersion}"
    }
}`;
    if (!contents.includes('com.mapbox.maps:android:')) {
      contents = contents + '\n' + resolutionBlock;
    }

    mod.modResults.contents = contents;
    return mod;
  });

  // ── Android: AndroidManifest ─────────────────────────────────────────────
  config = withAndroidManifest(config, (mod) => {
    const manifest = mod.modResults.manifest;

    // ── Permissions required by Mapbox Navigation SDK (targetSdk 35) ────────
    //
    // SecurityException: Starting FGS with type location requires:
    //   - android.permission.FOREGROUND_SERVICE_LOCATION   (mandatory API 34+)
    //   - android.permission.FOREGROUND_SERVICE             (mandatory)
    //   - android.permission.ACCESS_FINE_LOCATION           (at least one of coarse/fine)
    //   - android.permission.ACCESS_COARSE_LOCATION
    //
    const requiredPermissions = [
      'android.permission.ACCESS_FINE_LOCATION',
      'android.permission.ACCESS_COARSE_LOCATION',
      'android.permission.FOREGROUND_SERVICE',
      'android.permission.FOREGROUND_SERVICE_LOCATION',  // ← fixes the crash
      'android.permission.POST_NOTIFICATIONS',            // needed for nav notification (API 33+)
    ];

    if (!manifest['uses-permission']) {
      manifest['uses-permission'] = [];
    }

    for (const permission of requiredPermissions) {
      const already = manifest['uses-permission'].some(
        (p) => p.$?.['android:name'] === permission
      );
      if (!already) {
        manifest['uses-permission'].push({ $: { 'android:name': permission } });
      }
    }

    // ── Mapbox access token meta-data ────────────────────────────────────────
    const mainApp = manifest.application?.[0];
    if (mainApp) {
      if (!mainApp['meta-data']) mainApp['meta-data'] = [];
      const tokenMeta = mainApp['meta-data'].find(
        (m) => m.$?.['android:name'] === 'com.mapbox.token'
      );
      if (!tokenMeta) {
        mainApp['meta-data'].push({
          $: { 'android:name': 'com.mapbox.token', 'android:value': accessToken },
        });
      }
    }

    return mod;
  });

  // ── Android: color overrides ─────────────────────────────────────────────
  if (Object.keys(androidColorOverrides).length > 0) {
    config = withDangerousMod(config, [
      'android',
      async (mod) => {
        const colorsDir = path.join(
          mod.modRequest.platformProjectRoot,
          'app/src/main/res/values'
        );
        if (!fs.existsSync(colorsDir)) fs.mkdirSync(colorsDir, { recursive: true });

        const colorEntries = Object.entries(androidColorOverrides)
          .map(([name, value]) => `    <color name="${name}">${value}</color>`)
          .join('\n');

        fs.writeFileSync(
          path.join(colorsDir, 'mapbox_navigation_colors.xml'),
          `<?xml version="1.0" encoding="utf-8"?>\n<resources>\n${colorEntries}\n</resources>\n`
        );
        return mod;
      },
    ]);
  }

  // ── iOS: Info.plist ──────────────────────────────────────────────────────
  config = withInfoPlist(config, (mod) => {
    mod.modResults.MBXAccessToken = accessToken;

    if (!mod.modResults.NSLocationWhenInUseUsageDescription) {
      mod.modResults.NSLocationWhenInUseUsageDescription =
        'Your location is used for turn-by-turn navigation.';
    }
    if (!mod.modResults.NSLocationAlwaysAndWhenInUseUsageDescription) {
      mod.modResults.NSLocationAlwaysAndWhenInUseUsageDescription =
        'Your location is used for turn-by-turn navigation even when the app is in the background.';
    }
    if (!mod.modResults.UIBackgroundModes) mod.modResults.UIBackgroundModes = [];
    for (const mode of ['audio', 'location']) {
      if (!mod.modResults.UIBackgroundModes.includes(mode)) {
        mod.modResults.UIBackgroundModes.push(mode);
      }
    }
    return mod;
  });

  // ── iOS: .netrc for SPM authentication ───────────────────────────────────
  // Swift Package Manager (used by ExpoMapboxNavigation.podspec's
  // spm_dependency() to fetch the Mapbox Navigation SDK) authenticates
  // against api.mapbox.com using a machine entry in the user's ~/.netrc
  // file — this is Mapbox's documented mechanism for private SPM package
  // access, identical in spirit to the MAPBOX_DOWNLOADS_TOKEN gradle
  // property used on Android.
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
      }

      return mod;
    },
  ]);

  return config;
};

module.exports = withMapboxNavigation;
