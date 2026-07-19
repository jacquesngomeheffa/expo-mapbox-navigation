const {
  withAppBuildGradle,
  withProjectBuildGradle,
  withAndroidManifest,
  withGradleProperties,
  withInfoPlist,
  withDangerousMod,
  withXcodeProject,
} = require('@expo/config-plugins');
const fs = require('fs');
const path = require('path');

const withMapboxNavigation = (config, options = {}) => {
  const {
    accessToken,
    downloadsToken,
    // Android-only. iOS's MapboxMaps version is controlled separately by
    // `iosMapboxMapsVersion` below (5.0.5+) — the two platforms' correct
    // version pairings are NOT the same (Android has the -ndk27 artifact
    // constraint, iOS has the mapbox-navigation-ios-build-artifacts tag
    // constraint), so they are deliberately independent options.
    mapboxMapsVersion = '11.11.0',
    // iOS-only (5.0.5+). Version tag of Mapbox's official
    // mapbox-navigation-ios-build-artifacts repo that
    // ios/fetch-xcframeworks.sh clones at `pod install` time (vendored
    // Navigation binaries: MapboxNavigationCore, MapboxNavigationUIKit,
    // MapboxDirections + helpers, and the transitive
    // MapboxNavigationNative). Used EXACTLY as given — NO auto-calculation
    // on iOS, ever: YOU are responsible for verifying that this pairs
    // correctly with iosMapboxMapsVersion (check Mapbox's
    // mapbox-navigation-ios release notes for the MapboxMaps version each
    // Navigation release was built against). A mismatched pair means
    // link errors or runtime ABI crashes. The defaults are this package's
    // confirmed-working interlocked set:
    //   nav 3.20.1 / MapboxNavigationNative 324.20.2 /
    //   MapboxCommon 24.20.2 / MapboxMaps 11.20.2
    iosMapboxNavigationVersion = '3.20.1',
    // iOS-only (5.0.5+). The exact MapboxMaps version the podspec declares
    // (`s.dependency 'MapboxMaps', ...`), resolved from CocoaPods trunk —
    // NOT vendored by this package (it arrives via @rnmapbox/maps'
    // dependency chain). MUST equal the `RNMapboxMapsVersion` you set in
    // @rnmapbox/maps' own plugin config: if they differ, `pod install`
    // fails loudly with a version conflict — that failure is a protection,
    // not a bug. Must also pair correctly with iosMapboxNavigationVersion
    // (see above).
    iosMapboxMapsVersion = '11.20.2',
    // Android only (see calculateAndroidNavVersion below / withProjectBuildGradle
    // mod). If set, used EXACTLY as given — no auto-calculation, no safety
    // net. If omitted, derived from mapboxMapsVersion via the Phase 1/Phase 2
    // formula. Reactivated for 2.3.7 — previously deprecated/unused after
    // the 2.3.0 iOS rewrite removed its old (different) purpose.
    mapboxNavigationVersion = null,
    // Android only. Opt-in, default false (zero behavior change unless set).
    // Switches all 6 Mapbox Gradle dependencies to their "-ndk27" variant
    // for 16 KB page-size support (Google Play requirement, Android 15+).
    // Only exists for Navigation SDK >= 3.11.0 / Maps SDK >= 11.7.0 — you
    // MUST also pass a verified mapboxNavigationVersion/mapboxMapsVersion
    // pair at or above those, or Gradle will fail with "Could not resolve"
    // for an artifact that doesn't exist at your pinned version. See
    // "16 KB Page Size" in the README before enabling this.
    androidUseNdk27 = false,
    androidColorOverrides = {},
    // Read here specifically to sync into androidColorOverrides' generated
    // XML resources (mapbox_main_maneuver_background_color, in both a
    // values/ and values-night/ variant — see addAndroidConfig() below).
    // Every other color prop is passed directly as a view prop to native
    // (via src/index.tsx's `{...props}` spread) and never needs to be read
    // by this config plugin at all — these three are a deliberate
    // exception.
    //
    // FIX: `maneuverTurnIconColor` added to that exception list. Per
    // Mapbox's own official customization guide (Android Navigation SDK,
    // "Change the color of maneuver turn icons"), the turn icon's color is
    // controlled EXCLUSIVELY through a build-time XML style chain
    // (MapboxCustomManeuverTurnIconStyle, referenced from
    // MapboxCustomManeuverStyle via the maneuverViewIconStyle /
    // laneGuidanceManeuverIconStyle attributes) — there is no runtime
    // equivalent for icon color the way ManeuverViewOptions.
    // maneuverBackgroundColor exists for the background. The native
    // Kotlin-side ManeuverViewOptions.turnIconManeuver() call this package
    // already made was therefore never able to work on its own — this was
    // a genuine missing implementation, not a data-flow bug (confirmed:
    // native-side diagnostic logging showed the color correctly parsed and
    // handed to the SDK every time, with resId=0/"not generated" for the
    // custom style — see the maneuver color investigation). Reading it
    // here, generating the missing style chain below, closes that gap.
    maneuverBackgroundColorDay,
    maneuverBackgroundColorNight,
    maneuverTurnIconColor,
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
    addAndroidConfig(mod, mapboxMapsVersion, androidColorOverrides, maneuverBackgroundColorDay, maneuverBackgroundColorNight, maneuverTurnIconColor);
    return mod;
  });

  // ── Android: dynamic Mapbox Navigation SDK version ─────────────────────────
  // Writes `ext.mapboxMapsVersion` / `ext.mapboxNavVersion` into the app's
  // OWN root-level android/build.gradle, which this package's own
  // android/build.gradle then reads via `safeExtGet('mapboxNavVersion', ...)`
  // — the same standard Expo/RN idiom already used there for
  // compileSdkVersion/minSdkVersion/etc. This is what actually makes the
  // Gradle dependency versions configurable; setting `mapboxMapsVersion` in
  // app.json alone does NOT do this on its own (see 2.3.7 changelog — this
  // was previously hardcoded and silently ignored any app.json value).
  //
  // Logic, per explicit request: if BOTH mapboxMapsVersion AND
  // mapboxNavigationVersion are given, use both exactly as provided — no
  // recalculation, full trust in the caller (this is the safe path: pin an
  // exact pair you've actually verified against Mapbox's own release notes).
  // If ONLY mapboxMapsVersion is given, mapboxNavVersion is auto-derived via
  // the Phase 1/Phase 2 formula documented in the README. That formula is a
  // best-effort approximation, not a guarantee — Mapbox's own patch releases
  // don't always follow it exactly (see the "Patch versions drift" note in
  // this package's iOS history). Prefer passing both explicitly once you
  // know a working pair.
  config = withProjectBuildGradle(config, (mod) => {
    const navVersion = mapboxNavigationVersion || calculateAndroidNavVersion(mapboxMapsVersion, androidUseNdk27);
    if (!mod.modResults.contents.includes('ext.mapboxMapsVersion')) {
      // FIX #2 (see 2.3.8/2.3.9 changelog for FIX #1, which was itself
      // still broken): the real user-reported root android/build.gradle
      // has NO literal `ext { ... }` block anywhere in it at all — modern
      // Expo templates set compileSdkVersion/minSdkVersion/etc. via a
      // custom Gradle plugin (`expo-root-project` /
      // `com.facebook.react.rootproject`, applied near the bottom of the
      // file) that injects them programmatically, not via a plain-text
      // `ext { }` DSL block. So there was nothing for ANY regex
      // (`rootProject\.ext\s*{` or `\bext\s*{`) to match — confirmed by
      // directly inspecting a real, user-provided root build.gradle.
      //
      // Fix: stop trying to find-and-inject into a block that may not
      // exist at all. Just APPEND a brand new `ext { }` block at the end
      // of the file instead — the same robust, no-fragile-matching
      // approach `addAndroidConfig`'s dependencySubstitution injection
      // already uses successfully elsewhere in this same plugin (plain
      // `+=`, confirmed working via the user's own build.gradle showing
      // that block correctly present). Property values set via `ext { }`
      // become available as `rootProject.ext.X` for the rest of the
      // build regardless of where in the root script they're declared,
      // so appending at the end works the same as inserting near the top.
      mod.modResults.contents += `
ext {
    mapboxMapsVersion = "${mapboxMapsVersion}"
    mapboxNavVersion = "${navVersion}"
    mapboxUseNdk27 = ${androidUseNdk27 === true}
}
`;
      console.log(
        `[@jacques_gordon/expo-mapbox-navigation] Android: mapboxMapsVersion=${mapboxMapsVersion}, mapboxNavVersion=${navVersion}, ndk27=${androidUseNdk27 === true}` +
          (mapboxNavigationVersion ? ' (nav version explicit)' : ' (nav version auto-calculated — pass mapboxNavigationVersion to override)')
      );
    }
    return mod;
  });

  // ── Android: Mapbox Maven credentials (5.0.6) ─────────────────────────────
  // Writes MAPBOX_DOWNLOADS_TOKEN into android/gradle.properties - the
  // property that the api.mapbox.com Maven repository block (injected into
  // the project build.gradle by @rnmapbox/maps' own plugin) reads as its
  // password. Previously this package silently relied on @rnmapbox/maps'
  // plugin writing that property from its `RNMapboxMapsDownloadToken`
  // option - so when a consumer dropped that (deprecated) option, EVERY
  // api.mapbox.com Maven download 401'd, with this package's
  // com.mapbox.navigationcore:* artifacts as the first casualties (a real
  // EAS build failure, July 2026). Now this package provisions the
  // credential itself, from sources it controls:
  //   1. this plugin's own `downloadsToken` option (required anyway - the
  //      same sk.* token the iOS ~/.netrc mod uses);
  //   2. fallback: the RNMAPBOX_MAPS_DOWNLOAD_TOKEN environment variable
  //      (the EAS-secret-friendly name @rnmapbox/maps documents).
  // A NON-EMPTY value already present in gradle.properties (e.g. written
  // by @rnmapbox/maps' plugin when the consumer still uses its option, in
  // either plugin order) is respected and left untouched - this is a
  // safety net, not a takeover.
  config = withGradleProperties(config, (mod) => {
    const mavenToken = downloadsToken || process.env.RNMAPBOX_MAPS_DOWNLOAD_TOKEN;
    if (!mavenToken) return mod;
    const existing = mod.modResults.find(
      (item) => item.type === 'property' && item.key === 'MAPBOX_DOWNLOADS_TOKEN'
    );
    if (existing && existing.value) return mod;
    if (existing) {
      existing.value = mavenToken;
    } else {
      mod.modResults.push({ type: 'property', key: 'MAPBOX_DOWNLOADS_TOKEN', value: mavenToken });
    }
    console.log(
      '[@jacques_gordon/expo-mapbox-navigation] ✅ Wrote MAPBOX_DOWNLOADS_TOKEN to android/gradle.properties (Mapbox Maven credentials)'
    );
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

  // ── iOS: force the app's own Xcode project deployment target ──────────────
  // This is the actual fix for "compiling for iOS 14.0, but module
  // 'MapboxMaps' has a minimum deployment target of iOS 15.1" — a whole
  // family of earlier attempts at this fix (2.3.4, 2.3.5) targeted
  // ExpoMapboxNavigation.podspec's own `s.platforms` / pod_target_xcconfig,
  // which govern CocoaPods-level dependency validation but do NOT control
  // the value actually used to compile the app's own target. That value
  // lives directly in the consuming app's own Xcode project file
  // (ios/<AppName>.xcodeproj/project.pbxproj, generated by `expo
  // prebuild`), completely outside CocoaPods/Podfile territory — confirmed
  // by multiple independent reports of this exact error format needing a
  // direct project.pbxproj edit, not a Podfile/podspec change (see 2.3.6
  // changelog for sources). The standard Expo-recommended way to control
  // this is expo-build-properties' `ios.deploymentTarget`, but that
  // requires the consuming app to configure it correctly themselves. Since
  // our own vendored frameworks hard-require 15.1 regardless, we enforce
  // it directly here rather than just documenting it, so this can't be
  // silently misconfigured downstream.
  const MINIMUM_IOS_DEPLOYMENT_TARGET = '15.1';
  config = withXcodeProject(config, (mod) => {
    const xcodeProject = mod.modResults;
    const configurations = xcodeProject.pbxXCBuildConfigurationSection();
    let changedCount = 0;
    for (const key in configurations) {
      const entry = configurations[key];
      const buildSettings = entry && entry.buildSettings;
      if (!buildSettings || !('IPHONEOS_DEPLOYMENT_TARGET' in buildSettings)) continue;
      const raw = String(buildSettings['IPHONEOS_DEPLOYMENT_TARGET']).replace(/"/g, '');
      const current = parseFloat(raw);
      if (Number.isNaN(current) || current < parseFloat(MINIMUM_IOS_DEPLOYMENT_TARGET)) {
        buildSettings['IPHONEOS_DEPLOYMENT_TARGET'] = MINIMUM_IOS_DEPLOYMENT_TARGET;
        changedCount++;
      }
    }
    if (changedCount > 0) {
      console.log(
        `[@jacques_gordon/expo-mapbox-navigation] ✅ Raised IPHONEOS_DEPLOYMENT_TARGET to ${MINIMUM_IOS_DEPLOYMENT_TARGET} on ${changedCount} build configuration(s) in the Xcode project`
      );
    }
    return mod;
  });

  // ── iOS: .netrc for Mapbox downloads authentication ───────────────────────
  // REQUIRED again as of the on-demand-fetch architecture: this package's
  // own xcframeworks are no longer committed to its npm package/git repo
  // (see ExpoMapboxNavigation.podspec's s.prepare_command and
  // ios/fetch-xcframeworks.sh) — they're downloaded directly into the
  // CONSUMING app's own ios/Frameworks/ during that app's own `pod
  // install`, which needs valid Mapbox DOWNLOADS:READ credentials at that
  // point. Written during `expo prebuild`, which always runs before `pod
  // install` in a normal build flow, so this is in place in time.
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

  // ── iOS: dynamic SDK version selection (5.0.5+) ───────────────────────────
  // Writes the chosen iOS versions into a small JSON file INSIDE this
  // package's own installed directory (node_modules/.../ios/), during
  // `expo prebuild` — which always runs before `pod install`, so both
  // readers find it in time:
  //   - ExpoMapboxNavigation.podspec reads iosMapboxMapsVersion for its
  //     `s.dependency 'MapboxMaps', ...` line;
  //   - ios/fetch-xcframeworks.sh reads iosMapboxNavigationVersion for
  //     the artifacts tag to clone (unless the MAPBOX_NAV_VERSION env var
  //     overrides it — CI escape hatch, kept from before).
  // If this file is absent (bare `pod install` without prebuild, older
  // consumers), both readers fall back to the same hardcoded defaults —
  // exactly the pre-5.0.5 behavior, so this is zero-regression by
  // construction.
  config = withDangerousMod(config, [
    'ios',
    (mod) => {
      const semverRe = /^\d+\.\d+\.\d+$/;
      for (const [name, value] of [
        ['iosMapboxNavigationVersion', iosMapboxNavigationVersion],
        ['iosMapboxMapsVersion', iosMapboxMapsVersion],
      ]) {
        if (!semverRe.test(String(value))) {
          throw new Error(
            `[@jacques_gordon/expo-mapbox-navigation] \`${name}\` must be an exact semver like "3.20.1" — got "${value}".`
          );
        }
      }
      // plugin/src/index.js → package root is two levels up.
      const packageIosDir = path.join(__dirname, '..', '..', 'ios');
      if (fs.existsSync(packageIosDir)) {
        const versionsPath = path.join(packageIosDir, '.mapbox-ios-versions.json');
        fs.writeFileSync(
          versionsPath,
          JSON.stringify({ iosMapboxNavigationVersion, iosMapboxMapsVersion }, null, 2) + '\n'
        );
        console.log(
          `[@jacques_gordon/expo-mapbox-navigation] ✅ iOS SDK versions pinned: navigation ${iosMapboxNavigationVersion} / MapboxMaps ${iosMapboxMapsVersion}`
        );
      }
      return mod;
    },
  ]);

  return config;
};

// ── Android helpers ───────────────────────────────────────────────────────────

// Reconstructed from this package's earlier (2.2.x-era) iOS version-pairing
// research — Mapbox's Navigation/Maps SDK release pattern, confirmed from
// their own GitHub release history at the time:
//   Phase 1 (Nav 3.1–3.12): navMinor = mapsMinor - 3
//   Phase 2 (Nav 3.16+):    navMinor = mapsMinor (aligned)
//   Nav 3.13–3.15 were deliberately skipped by Mapbox entirely, which is why
//   the crossover point is mapsMinor 16, not mapsMinor 15 (15 - 3 = 12,
//   still valid Phase 1; 16 - 3 = 13, which doesn't exist, so Phase 2 takes
//   over exactly there instead).
// This is a best-effort APPROXIMATION, not a guarantee — Mapbox's patch
// releases don't always follow it exactly. Pass `mapboxNavigationVersion`
// explicitly once you've verified a working pair against Mapbox's release
// notes, rather than relying on this for anything beyond a starting point.
function calculateAndroidNavVersion(mapboxMapsVersion, requireNdk27 = false) {
  const parts = String(mapboxMapsVersion).split('.');
  const mapsMinor = parseInt(parts[1], 10);
  if (Number.isNaN(mapsMinor)) {
    // Can't parse — fall back to a safe default. If ndk27 is required,
    // '3.8.1' (this package's originally shipped pairing) would fail to
    // resolve as "-ndk27" (doesn't exist below 3.11.0), so fall back to
    // '3.11.0' — the lowest version where -ndk27 artifacts exist at all —
    // instead in that case.
    return requireNdk27 ? '3.11.0' : '3.8.1';
  }
  let navMinor = mapsMinor <= 15 ? mapsMinor - 3 : mapsMinor;
  // -ndk27 artifacts only exist for Navigation SDK >= 3.11.0 (confirmed
  // from Mapbox's own changelog). If androidUseNdk27 is enabled and the
  // Phase 1/Phase 2 formula would otherwise derive a version below that
  // (e.g. mapboxMapsVersion "11.11.0" → navMinor 8, i.e. "3.8.0" — this
  // package's own historical default pairing), floor it at 11 instead of
  // producing a version guaranteed to fail with "Could not resolve
  // com.mapbox.navigationcore:...-ndk27:...". This is only a safety floor
  // for the AUTO-CALCULATED case — passing mapboxNavigationVersion
  // explicitly always takes priority over this function entirely (see
  // withProjectBuildGradle below).
  if (requireNdk27 && navMinor < 11) {
    navMinor = 11;
  }
  return `3.${navMinor}.0`;
}

// Mapbox Maps SDK and Common SDK follow a SYNCHRONIZED versioning scheme —
// only the major version differs (11.x.y for Maps, 24.x.y for Common),
// minor and patch are always identical. Confirmed against Mapbox's own
// compatibility table (11.14.0→24.14.0, 11.14.2→24.14.2, 11.14.8→24.14.8,
// etc. — every entry checked, no exceptions found). Replaces a previously
// hardcoded '24.11.3', which was already slightly wrong even for the
// default mapboxMapsVersion ('11.11.0' → should be '24.11.0', not
// '24.11.3') — this was a real bug, not just a missing convenience.
function calculateMapboxCommonVersion(mapboxMapsVersion) {
  const parts = String(mapboxMapsVersion).split('.');
  if (parts[0] !== '11' || parts.length < 2) {
    // Can't confidently compute — fall back to this package's originally
    // shipped value rather than guessing wrong.
    return '24.11.3';
  }
  return ['24', ...parts.slice(1)].join('.');
}

function addAndroidConfig(mod, mapboxMapsVersion, androidColorOverrides, maneuverBackgroundColorDay, maneuverBackgroundColorNight, maneuverTurnIconColor) {
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

  // ── 16 KB page size — Maps/Common (unconditional) ─────────────────────────
  // This substitution runs on EVERY prebuild, regardless of the
  // `androidUseNdk27` option (that option only governs the SEPARATE
  // com.mapbox.navigationcore:* substitution in this package's own
  // android/build.gradle — different Mapbox artifact group, different
  // mechanism, intentionally independent). Kept unconditional here since
  // this behavior already existed and apps may already be relying on it.
  //
  // MAPS_VER: dynamic (mapboxMapsVersion option, default '11.11.0').
  // COMMON_VER: now also dynamic — see calculateMapboxCommonVersion above.
  // Previously hardcoded to '24.11.3', which didn't even correctly match
  // the default Maps version (should've been '24.11.0').
  if (!mod.modResults.contents.includes('dependencySubstitution')) {
    const MAPS_VER = mapboxMapsVersion || '11.11.0';
    const COMMON_VER = calculateMapboxCommonVersion(MAPS_VER);
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
  //
  // FIX: `maneuverBackgroundColorDay` (this package's own prop, applied at
  // RUNTIME via ManeuverViewOptions) and `androidColorOverrides.
  // mapbox_main_maneuver_background_color` (a real, intentional mechanism
  // documented in this package's own upstream/origin project — a BUILD-TIME
  // Android resource override) both target the same visual element, via two
  // completely independent mechanisms. If an app sets both to DIFFERENT
  // values, whichever one Mapbox's SDK actually resolves first wins
  // silently, making the OTHER one appear broken — exactly the symptom
  // reported. Since `mapbox_main_maneuver_background_color` is the
  // resource name this specific fork's ORIGIN project already documents
  // and other apps may already rely on, we sync `maneuverBackgroundColorDay`
  // INTO it automatically here — UNLESS the app has already explicitly set
  // that exact key itself in androidColorOverrides, in which case their
  // explicit value is respected instead (never silently overridden).
  //
  // FIX (3.1.5): both the color resource AND the style below are now
  // generated TWICE — once in res/values/ (day) and once in
  // res/values-night/ (night), using Android's own standard resource
  // qualifier system. Root cause, confirmed directly on a real device:
  // Mapbox ships its own values-night/ qualified resources for
  // MapboxStyleManeuverView, which Android automatically selects over this
  // package's own PREVIOUSLY non-qualified (values/ only) style whenever
  // the system is in dark mode — regardless of what either color mechanism
  // sets programmatically. Providing our own values-night/ variant, using
  // the exact same resource names, lets Android's own resource merging
  // correctly prefer THIS package's override over Mapbox's default in
  // dark mode too, the same way it already did in light mode.
  //
  // Night resource priority mirrors the native ManeuverView's own fallback
  // logic exactly (resolveManeuverBackgroundColor() in
  // ExpoMapboxNavigationView.kt): explicit androidColorOverrides value (if
  // set — applies to both day and night, since androidColorOverrides has
  // no separate night-specific key of its own) > maneuverBackgroundColorNight
  // > maneuverBackgroundColorDay (fallback) > nothing written.
  const dayColorOverrides = { ...androidColorOverrides };
  if (
    maneuverBackgroundColorDay &&
    !Object.prototype.hasOwnProperty.call(dayColorOverrides, 'mapbox_main_maneuver_background_color')
  ) {
    dayColorOverrides.mapbox_main_maneuver_background_color = maneuverBackgroundColorDay;
  }

  const nightColorOverrides = { ...androidColorOverrides };
  if (!Object.prototype.hasOwnProperty.call(nightColorOverrides, 'mapbox_main_maneuver_background_color')) {
    const nightValue = maneuverBackgroundColorNight || maneuverBackgroundColorDay;
    if (nightValue) {
      nightColorOverrides.mapbox_main_maneuver_background_color = nightValue;
    }
  }

  function writeValuesFile(qualifier, fileName, contents) {
    if (!contents) return;
    const dir = path.join(
      mod.modRequest?.platformProjectRoot || '',
      'app', 'src', 'main', 'res', qualifier
    );
    try {
      fs.mkdirSync(dir, { recursive: true });
      fs.writeFileSync(path.join(dir, fileName), contents);
    } catch (e) {
      // Ignore — resDir may not exist at plugin resolution time
    }
  }

  function colorOverridesXml(overrides) {
    if (Object.keys(overrides).length === 0) return null;
    const colorEntries = Object.entries(overrides)
      .map(([name, value]) => `    <color name="${name}">${value}</color>`)
      .join('\n');
    return `<?xml version="1.0" encoding="utf-8"?>\n<resources>\n${colorEntries}\n</resources>\n`;
  }

  writeValuesFile('values', 'mapbox_color_overrides.xml', colorOverridesXml(dayColorOverrides));
  writeValuesFile('values-night', 'mapbox_color_overrides.xml', colorOverridesXml(nightColorOverrides));

  // Generate MapboxCustomManeuverStyle (see 3.1.4 changelog for the full
  // reasoning behind this XML-style-attribute-based mechanism, applied via
  // ContextThemeWrapper on the native side, alongside — not instead of —
  // the ManeuverViewOptions-based approach). Same day/night duplication as
  // the color resources above, and for the exact same reason.
  //
  // FIX: also generates the turn-icon-color style chain
  // (MapboxCustomManeuverTurnIconStyle, referenced via
  // maneuverViewIconStyle/laneGuidanceManeuverIconStyle), per Mapbox's own
  // official Android Navigation SDK guide ("Change the color of maneuver
  // turn icons") — confirmed against that guide's exact attribute/style
  // names, not guessed. This was the missing piece: maneuverTurnIconColor
  // has no runtime equivalent to ManeuverViewOptions.maneuverBackgroundColor
  // for icon color, so the native-side ManeuverViewOptions.turnIconManeuver()
  // call alone could never visibly work, regardless of how correctly it
  // received and parsed the color (confirmed via diagnostic logging during
  // the investigation into this exact prop).
  //
  // maneuverTurnIconColor has no separate day/night prop of its own (unlike
  // the background color) — the SAME value is used for both values/ and
  // values-night/ generation below, for the same reason the background
  // color needs both: Mapbox ships its own values-night/ qualified
  // defaults for MapboxStyleTurnIconManeuver too, which would otherwise
  // silently win over a values/-only override in dark mode.
  //
  // No regression for existing apps using only the background color props:
  // when maneuverTurnIconColor is not set, the generated XML is byte-for-byte
  // identical to before this fix (no icon style block, no icon-referencing
  // items on MapboxCustomManeuverStyle).
  function maneuverStyleXml(overrides, turnIconColor) {
    const bgColor = overrides.mapbox_main_maneuver_background_color;
    if (!bgColor && !turnIconColor) return null;

    const bgItems = bgColor
      ? '        <item name="maneuverViewBackgroundColor">@color/mapbox_main_maneuver_background_color</item>\n' +
        '        <item name="subManeuverViewBackgroundColor">@color/mapbox_main_maneuver_background_color</item>\n' +
        '        <item name="upcomingManeuverViewBackgroundColor">@color/mapbox_main_maneuver_background_color</item>\n'
      : '';

    const iconStyleBlock = turnIconColor
      ? `    <style name="MapboxCustomManeuverTurnIconStyle" parent="MapboxStyleTurnIconManeuver">\n` +
        `        <item name="maneuverTurnIconColor">${turnIconColor}</item>\n` +
        `    </style>\n`
      : '';

    const iconRefItems = turnIconColor
      ? '        <item name="maneuverViewIconStyle">@style/MapboxCustomManeuverTurnIconStyle</item>\n' +
        '        <item name="laneGuidanceManeuverIconStyle">@style/MapboxCustomManeuverTurnIconStyle</item>\n'
      : '';

    return `<?xml version="1.0" encoding="utf-8"?>
<resources>
${iconStyleBlock}    <style name="MapboxCustomManeuverStyle" parent="MapboxStyleManeuverView">
${bgItems}${iconRefItems}    </style>
</resources>
`;
  }

  writeValuesFile('values', 'mapbox_maneuver_style.xml', maneuverStyleXml(dayColorOverrides, maneuverTurnIconColor));
  writeValuesFile('values-night', 'mapbox_maneuver_style.xml', maneuverStyleXml(nightColorOverrides, maneuverTurnIconColor));
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

  if (!application.service) application.service = [];
  const svcName = 'com.mapbox.navigation.core.trip.service.NavigationNotificationService';
  if (!application.service.find((s) => s['$']['android:name'] === svcName)) {
    application.service.push({
      $: {
        'android:name': svcName,
        'android:foregroundServiceType': 'location',
        'android:exported': 'false',
      },
    });
  }
}

module.exports = withMapboxNavigation;
