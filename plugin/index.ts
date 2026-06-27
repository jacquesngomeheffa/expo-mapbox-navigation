import {
  ConfigPlugin,
  withInfoPlist,
  withAppBuildGradle,
  withProjectBuildGradle,
  withAndroidManifest,
  createRunOncePlugin,
} from '@expo/config-plugins';

export interface ExpoMapboxNavigationPluginOptions {
  /** Token public Mapbox (commence par pk.) */
  accessToken: string;
  /**
   * Version du SDK Mapbox Maps (doit correspondre à la version utilisée par @rnmapbox/maps).
   * Exemple: "11.11.0"
   */
  mapboxMapsVersion?: string;
  /**
   * Overrides de couleurs Android pour personnaliser l'UI Mapbox.
   * Exemple: { "mapbox_main_maneuver_background_color": "#FF0000" }
   */
  androidColorOverrides?: Record<string, string>;
}

const withExpoMapboxNavigation: ConfigPlugin<ExpoMapboxNavigationPluginOptions> = (
  config,
  options
) => {
  const { accessToken, mapboxMapsVersion = '11.11.0', androidColorOverrides = {} } = options;

  if (!accessToken) {
    throw new Error(
      '[expo-mapbox-navigation] accessToken est requis dans la configuration du plugin.'
    );
  }

  // ── iOS — Info.plist ──────────────────────────────────────────────────────

  config = withInfoPlist(config, (cfg) => {
    cfg.modResults['MBXAccessToken'] = accessToken;

    cfg.modResults['NSLocationWhenInUseUsageDescription'] =
      cfg.modResults['NSLocationWhenInUseUsageDescription'] ??
      'Cette application utilise votre position pour la navigation GPS.';

    cfg.modResults['NSLocationAlwaysAndWhenInUseUsageDescription'] =
      cfg.modResults['NSLocationAlwaysAndWhenInUseUsageDescription'] ??
      "Cette application utilise votre position en arrière-plan pour la navigation GPS.";

    cfg.modResults['NSMotionUsageDescription'] =
      cfg.modResults['NSMotionUsageDescription'] ??
      "Cette application utilise le capteur de mouvement pour améliorer la navigation.";

    // Background modes: audio (voix) + location
    const uiBackgroundModes: string[] = cfg.modResults['UIBackgroundModes'] ?? [];
    if (!uiBackgroundModes.includes('audio')) uiBackgroundModes.push('audio');
    if (!uiBackgroundModes.includes('location')) uiBackgroundModes.push('location');
    cfg.modResults['UIBackgroundModes'] = uiBackgroundModes;

    return cfg;
  });

  // ── Android — project build.gradle (repo Maven Mapbox) ───────────────────

  config = withProjectBuildGradle(config, (cfg) => {
    if (!cfg.modResults.contents.includes('api.mapbox.com/downloads')) {
      // Ajouter le repo Mapbox dans allprojects > repositories
      cfg.modResults.contents = cfg.modResults.contents.replace(
        /allprojects\s*\{([\s\S]*?)repositories\s*\{/,
        `allprojects {$1repositories {
    maven {
      url 'https://api.mapbox.com/downloads/v2/releases/maven'
      authentication { basic(BasicAuthentication) }
      credentials {
        username = "mapbox"
        password = project.properties['MAPBOX_DOWNLOADS_TOKEN'] ?: System.getenv("MAPBOX_DOWNLOADS_TOKEN") ?: ""
      }
    }`
      );
    }
    return cfg;
  });

  // ── Android — app build.gradle (token + version maps) ────────────────────

  config = withAppBuildGradle(config, (cfg) => {
    // Injecter le token Mapbox public comme ressource string
    if (!cfg.modResults.contents.includes('mapbox_access_token')) {
      cfg.modResults.contents = cfg.modResults.contents.replace(
        /defaultConfig\s*\{/,
        `defaultConfig {
    resValue "string", "mapbox_access_token", "${accessToken}"`
      );
    }

    // Injecter RNMapboxMapsVersion (requis par @rnmapbox/maps)
    if (
      mapboxMapsVersion &&
      !cfg.modResults.contents.includes('RNMapboxMapsVersion')
    ) {
      cfg.modResults.contents = cfg.modResults.contents.replace(
        /ext\s*\{/,
        `ext {
    RNMapboxMapsVersion = "${mapboxMapsVersion}"`
      );

      // Si pas de bloc ext, en créer un
      if (!cfg.modResults.contents.includes('RNMapboxMapsVersion')) {
        cfg.modResults.contents = cfg.modResults.contents.replace(
          /^(buildscript\s*\{)/m,
          `ext {
  RNMapboxMapsVersion = "${mapboxMapsVersion}"
}

$1`
        );
      }
    }

    // Générer les color overrides Android
    if (Object.keys(androidColorOverrides).length > 0) {
      const colorResources = Object.entries(androidColorOverrides)
        .map(([name, value]) => `    resValue "color", "${name}", "${value}"`)
        .join('\n');

      if (!cfg.modResults.contents.includes('androidColorOverrides_injected')) {
        cfg.modResults.contents = cfg.modResults.contents.replace(
          /defaultConfig\s*\{/,
          `defaultConfig {
${colorResources}
    // androidColorOverrides_injected`
        );
      }
    }

    return cfg;
  });

  // ── Android — Manifest (permissions) ─────────────────────────────────────

  config = withAndroidManifest(config, (cfg) => {
    const manifest = cfg.modResults.manifest;
    const permissions: any[] = manifest['uses-permission'] ?? [];

    const addPermission = (name: string) => {
      if (!permissions.some((p: any) => p.$?.['android:name'] === name)) {
        permissions.push({ $: { 'android:name': name } });
      }
    };

    addPermission('android.permission.ACCESS_FINE_LOCATION');
    addPermission('android.permission.ACCESS_COARSE_LOCATION');
    addPermission('android.permission.FOREGROUND_SERVICE');
    addPermission('android.permission.FOREGROUND_SERVICE_LOCATION');

    manifest['uses-permission'] = permissions;

    // Ajouter le token Mapbox dans les métadonnées de l'application
    const application = manifest.application?.[0];
    if (application) {
      const metaData: any[] = application['meta-data'] ?? [];
      const existingToken = metaData.find(
        (m: any) => m.$?.['android:name'] === 'com.mapbox.token'
      );
      if (!existingToken) {
        metaData.push({
          $: {
            'android:name': 'com.mapbox.token',
            'android:value': accessToken,
          },
        });
      }
      application['meta-data'] = metaData;
    }

    return cfg;
  });

  return config;
};

export default createRunOncePlugin(
  withExpoMapboxNavigation,
  'expo-mapbox-navigation',
  '1.0.0'
);
