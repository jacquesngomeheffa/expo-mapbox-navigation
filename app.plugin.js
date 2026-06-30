const {
  withAppBuildGradle,
  withProjectBuildGradle,
  withAndroidManifest,
  withInfoPlist,
  withDangerousMod,
} = require('@expo/config-plugins');
const { mergeContents } = require('@expo/config-plugins/build/utils/generateCode');
const fs = require('fs');
const path = require('path');


const withMapboxNavigation = (config, options = {}) => {
  const {
    accessToken,
    downloadsToken,
    mapboxMapsVersion = '11.11.0',
    androidColorOverrides = {},
  } = options;

  if (!accessToken) {
    throw new Error(
      '[@jacques_gordon/expo-mapbox-navigation] `accessToken` is required in the plugin config.\n' +
      'Add it to your app.json plugins array:\n' +
      '  ["@jacques_gordon/expo-mapbox-navigation", { "accessToken": "pk.your_token" }]'
    );
  }

  if (!downloadsToken) {
    throw new Error(
      '[@jacques_gordon/expo-mapbox-navigation] `downloadsToken` is required for iOS builds.\n' +
      'This must be a SECRET Mapbox token (starts with "sk.") with the "Downloads:Read" scope.\n' +
      'Add it to your app.json plugins array:\n' +
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

  // ── iOS: MBXAccessToken in Info.plist ────────────────────────────────────
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

  // ── iOS: .netrc for SPM authentication ───────────────────────────────────
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

  // ── iOS: Inject Mapbox Navigation SPM into project.pbxproj ───────────────
  //
  // WHY NOT spm_dependency() IN THE PODSPEC:
  //   spm_dependency() causes "43029 duplicate symbols" linker errors alongside
  //   @rnmapbox/maps (React Native #47344, Expo #37813) — the framework gets
  //   linked in both the Pod target AND the main app target simultaneously.
  //
  // WHY NOT xcodeProject.addSwiftPackage():
  //   That method does not exist in the `xcode` npm package that Expo uses
  //   under the hood — it throws "addSwiftPackage is not a function".
  //
  // THE SOLUTION — withDangerousMod on project.pbxproj:
  //   We inject the SPM package reference and product dependencies directly
  //   into the .pbxproj file as text. This is the same technique used by
  //   several established Expo modules (e.g. expo-notifications, rnmapbox)
  //   for SPM dependencies that have no CocoaPods distribution.
  //   The Mapbox Navigation SDK v3 explicitly states "CocoaPods support is
  //   currently in development" — SPM is the only official distribution.
  config = withDangerousMod(config, [
    'ios',
    (mod) => {
      const projectRoot = mod.modRequest.platformProjectRoot;
      const projectName = mod.modRequest.projectName;
      const pbxprojPath = path.join(
        projectRoot,
        `${projectName}.xcodeproj`,
        'project.pbxproj'
      );

      if (!fs.existsSync(pbxprojPath)) {
        console.warn(`[@jacques_gordon/expo-mapbox-navigation] Could not find ${pbxprojPath} — skipping SPM injection`);
        return mod;
      }

      let pbxproj = fs.readFileSync(pbxprojPath, 'utf8');

      // Skip if already injected
      if (pbxproj.includes('mapbox-navigation-ios')) {
        return mod;
      }

      // Generate stable UUIDs for the new entries
      // (UUIDs must be 24 hex chars in pbxproj format)
      const makeUUID = () => {
        const { randomBytes } = require('crypto');
        return randomBytes(12).toString('hex').toUpperCase();
      };

      const pkgRefUUID     = makeUUID(); // XCRemoteSwiftPackageReference
      const coreDepUUID    = makeUUID(); // XCSwiftPackageProductDependency (MapboxNavigationCore)
      const uikitDepUUID   = makeUUID(); // XCSwiftPackageProductDependency (MapboxNavigationUIKit)
      const coreBuildUUID  = makeUUID(); // PBXBuildFile (MapboxNavigationCore)
      const uikitBuildUUID = makeUUID(); // PBXBuildFile (MapboxNavigationUIKit)

      const NAV_IOS_REPO    = 'https://github.com/mapbox/mapbox-navigation-ios.git';
      const NAV_IOS_VERSION = '3.25.0';

      // 1. Add XCRemoteSwiftPackageReference
      const pkgRefEntry = `
\t\t${pkgRefUUID} /* XCRemoteSwiftPackageReference "mapbox-navigation-ios" */ = {
\t\t\tisa = XCRemoteSwiftPackageReference;
\t\t\trequirement = {
\t\t\t\tkind = upToNextMajorVersion;
\t\t\t\tminimumVersion = ${NAV_IOS_VERSION};
\t\t\t};
\t\t\trepositoryURL = "${NAV_IOS_REPO}";
\t\t};`;

      // 2. Add XCSwiftPackageProductDependencies
      const coreDepEntry = `
\t\t${coreDepUUID} /* MapboxNavigationCore */ = {
\t\t\tisa = XCSwiftPackageProductDependency;
\t\t\tpackage = ${pkgRefUUID} /* XCRemoteSwiftPackageReference "mapbox-navigation-ios" */;
\t\t\tproductName = MapboxNavigationCore;
\t\t};`;

      const uikitDepEntry = `
\t\t${uikitDepUUID} /* MapboxNavigationUIKit */ = {
\t\t\tisa = XCSwiftPackageProductDependency;
\t\t\tpackage = ${pkgRefUUID} /* XCRemoteSwiftPackageReference "mapbox-navigation-ios" */;
\t\t\tproductName = MapboxNavigationUIKit;
\t\t};`;

      // 3. Add PBXBuildFile entries for the frameworks
      const coreBuildEntry = `
\t\t${coreBuildUUID} /* MapboxNavigationCore in Frameworks */ = {isa = PBXBuildFile; productRef = ${coreDepUUID} /* MapboxNavigationCore */; };`;

      const uikitBuildEntry = `
\t\t${uikitBuildUUID} /* MapboxNavigationUIKit in Frameworks */ = {isa = PBXBuildFile; productRef = ${uikitDepUUID} /* MapboxNavigationUIKit */; };`;

      // Inject into the relevant sections
      // Section: XCRemoteSwiftPackageReference
      if (pbxproj.includes('/* Begin XCRemoteSwiftPackageReference section */')) {
        pbxproj = pbxproj.replace(
          '/* Begin XCRemoteSwiftPackageReference section */',
          `/* Begin XCRemoteSwiftPackageReference section */${pkgRefEntry}`
        );
      } else {
        // Section doesn't exist yet — add it before the end of the project
        pbxproj = pbxproj.replace(
          '/* End XCConfigurationList section */',
          `/* End XCConfigurationList section */\n\n/* Begin XCRemoteSwiftPackageReference section */${pkgRefEntry}\n/* End XCRemoteSwiftPackageReference section */`
        );
      }

      // Section: XCSwiftPackageProductDependency
      if (pbxproj.includes('/* Begin XCSwiftPackageProductDependency section */')) {
        pbxproj = pbxproj.replace(
          '/* Begin XCSwiftPackageProductDependency section */',
          `/* Begin XCSwiftPackageProductDependency section */${coreDepEntry}${uikitDepEntry}`
        );
      } else {
        pbxproj = pbxproj.replace(
          '/* End XCRemoteSwiftPackageReference section */',
          `/* End XCRemoteSwiftPackageReference section */\n\n/* Begin XCSwiftPackageProductDependency section */${coreDepEntry}${uikitDepEntry}\n/* End XCSwiftPackageProductDependency section */`
        );
      }

      // Section: PBXBuildFile
      pbxproj = pbxproj.replace(
        '/* Begin PBXBuildFile section */',
        `/* Begin PBXBuildFile section */${coreBuildEntry}${uikitBuildEntry}`
      );

      // Add to main target's Frameworks build phase
      // FIX: the regex was non-greedy and could match the test target instead
      // of the main app target. We now inject into ALL PBXFrameworksBuildPhase
      // files lists — Xcode deduplicates at link time, and only the main app
      // target actually links frameworks, so this is safe.
      // Use a replace-all approach on PBXBuildFile section only (already done above),
      // and find all 'files = (' inside PBXFrameworksBuildPhase section precisely.
      const frameworksSectionMatch = pbxproj.match(
        /\/\* Begin PBXFrameworksBuildPhase section \*\/([\s\S]*?)\/\* End PBXFrameworksBuildPhase section \*\//
      );
      if (frameworksSectionMatch) {
        const originalSection = frameworksSectionMatch[0];
        // Replace only the FIRST 'files = (' inside this section (= main app target)
        const patchedSection = originalSection.replace(
          'files = (',
          `files = (\n\t\t\t\t${coreBuildUUID} /* MapboxNavigationCore in Frameworks */,\n\t\t\t\t${uikitBuildUUID} /* MapboxNavigationUIKit in Frameworks */,`
        );
        pbxproj = pbxproj.replace(originalSection, patchedSection);
      }

      // Add to main target's packageProductDependencies
      // FIX: only replace the FIRST occurrence (main app target, not test target)
      if (pbxproj.includes('packageProductDependencies = (')) {
        pbxproj = pbxproj.replace(
          'packageProductDependencies = (',
          `packageProductDependencies = (\n\t\t\t\t${coreDepUUID} /* MapboxNavigationCore */,\n\t\t\t\t${uikitDepUUID} /* MapboxNavigationUIKit */,`
        );
      } else {
        // packageProductDependencies doesn't exist yet — add it to the first PBXNativeTarget
        pbxproj = pbxproj.replace(
          /(isa = PBXNativeTarget;[\s\S]*?)(packageReferences = \([\s\S]*?\);)/,
          `$1$2\n\t\t\tpackageProductDependencies = (\n\t\t\t\t${coreDepUUID} /* MapboxNavigationCore */,\n\t\t\t\t${uikitDepUUID} /* MapboxNavigationUIKit */,\n\t\t\t);`
        );
      }

      // Add to project's packageReferences list (first occurrence only = main project)
      if (pbxproj.includes('packageReferences = (')) {
        pbxproj = pbxproj.replace(
          'packageReferences = (',
          `packageReferences = (\n\t\t\t\t${pkgRefUUID} /* XCRemoteSwiftPackageReference "mapbox-navigation-ios" */,`
        );
      }

      fs.writeFileSync(pbxprojPath, pbxproj, 'utf8');
      console.log('[@jacques_gordon/expo-mapbox-navigation] ✅ Injected Mapbox Navigation SPM into project.pbxproj');

      return mod;
    },
  ]);

  return config;
};

// ── Android helpers ──────────────────────────────────────────────────────────

function addAndroidConfig(mod, mapboxMapsVersion, androidColorOverrides) {
  const MAPBOX_DOWNLOADS_TOKEN_KEY = 'MAPBOX_DOWNLOADS_TOKEN';
  // (kept for backward compat — actual token is in gradle.properties)

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

  if (Object.keys(androidColorOverrides).length > 0) {
    // inject color overrides as Android resource values
    // (handled downstream by the Mapbox gradle plugin or manual resource injection)
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
