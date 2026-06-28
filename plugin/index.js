const {
  withInfoPlist,
  withAppBuildGradle,
  withProjectBuildGradle,
  withAndroidManifest,
  createRunOncePlugin,
} = require("@expo/config-plugins");

const withExpoMapboxNavigation = (config, options) => {
  const {
    accessToken,
    mapboxMapsVersion = "11.11.0",
    androidColorOverrides = {},
  } = options || {};

  if (!accessToken) {
    throw new Error(
      "[expo-mapbox-navigation] accessToken est requis dans la configuration du plugin."
    );
  }

  // ── iOS ──
  config = withInfoPlist(config, (cfg) => {
    cfg.modResults["MBXAccessToken"] = accessToken;

    cfg.modResults["NSLocationWhenInUseUsageDescription"] =
      cfg.modResults["NSLocationWhenInUseUsageDescription"] ??
      "Cette application utilise votre position pour la navigation GPS.";

    cfg.modResults["NSLocationAlwaysAndWhenInUseUsageDescription"] =
      cfg.modResults["NSLocationAlwaysAndWhenInUseUsageDescription"] ??
      "Cette application utilise votre position en arrière-plan pour la navigation GPS.";

    cfg.modResults["NSMotionUsageDescription"] =
      cfg.modResults["NSMotionUsageDescription"] ??
      "Cette application utilise le capteur de mouvement pour améliorer la navigation.";

    const uiBackgroundModes = cfg.modResults["UIBackgroundModes"] ?? [];
    if (!uiBackgroundModes.includes("audio")) uiBackgroundModes.push("audio");
    if (!uiBackgroundModes.includes("location"))
      uiBackgroundModes.push("location");
    cfg.modResults["UIBackgroundModes"] = uiBackgroundModes;

    return cfg;
  });

  // ── Android project build.gradle ──
  config = withProjectBuildGradle(config, (cfg) => {
    if (!cfg.modResults.contents.includes("api.mapbox.com/downloads")) {
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

  // ── Android app build.gradle ──
  config = withAppBuildGradle(config, (cfg) => {
    if (!cfg.modResults.contents.includes("mapbox_access_token")) {
      cfg.modResults.contents = cfg.modResults.contents.replace(
        /defaultConfig\s*\{/,
        `defaultConfig {
    resValue "string", "mapbox_access_token", "${accessToken}"`
      );
    }

    if (
      mapboxMapsVersion &&
      !cfg.modResults.contents.includes("RNMapboxMapsVersion")
    ) {
      cfg.modResults.contents = cfg.modResults.contents.replace(
        /ext\s*\{/,
        `ext {
    RNMapboxMapsVersion = "${mapboxMapsVersion}"`
      );

      if (!cfg.modResults.contents.includes("RNMapboxMapsVersion")) {
        cfg.modResults.contents = cfg.modResults.contents.replace(
          /^(buildscript\s*\{)/m,
          `ext {
  RNMapboxMapsVersion = "${mapboxMapsVersion}"
}

$1`
        );
      }
    }

    if (Object.keys(androidColorOverrides).length > 0) {
      const colorResources = Object.entries(androidColorOverrides)
        .map(([name, value]) => `    resValue "color", "${name}", "${value}"`)
        .join("\n");

      if (!cfg.modResults.contents.includes("androidColorOverrides_injected")) {
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

  // ── Android Manifest ──
  config = withAndroidManifest(config, (cfg) => {
    const manifest = cfg.modResults.manifest;
    const permissions = manifest["uses-permission"] ?? [];

    const addPermission = (name) => {
      if (!permissions.some((p) => p.$?.["android:name"] === name)) {
        permissions.push({ $: { "android:name": name } });
      }
    };

    addPermission("android.permission.ACCESS_FINE_LOCATION");
    addPermission("android.permission.ACCESS_COARSE_LOCATION");
    addPermission("android.permission.FOREGROUND_SERVICE");
    addPermission("android.permission.FOREGROUND_SERVICE_LOCATION");

    manifest["uses-permission"] = permissions;

    const application = manifest.application?.[0];
    if (application) {
      const metaData = application["meta-data"] ?? [];

      const existingToken = metaData.find(
        (m) => m.$?.["android:name"] === "com.mapbox.token"
      );

      if (!existingToken) {
        metaData.push({
          $: {
            "android:name": "com.mapbox.token",
            "android:value": accessToken,
          },
        });
      }

      application["meta-data"] = metaData;
    }

    return cfg;
  });

  return config;
};

const plugin = createRunOncePlugin(
  withExpoMapboxNavigation,
  "expo-mapbox-navigation",
  "1.0.7"
);

module.exports = plugin;
module.exports.default = plugin;