# @jacques_gordon/expo-mapbox-navigation

[![npm version](https://badge.fury.io/js/@jacques_gordon%2Fexpo-mapbox-navigation.svg)](https://www.npmjs.com/package/@jacques_gordon/expo-mapbox-navigation)

Full-featured Expo module for Mapbox Navigation SDK v3 — Android and iOS.

---

## Features

- **Android** — Waze-style navigation UI built from scratch: maneuver banner, lane guidance, speed limit, ETA bar, voice instructions, mute/overview/recenter buttons, day/night auto-switch
- **iOS** — Drop-in `NavigationViewController` from Mapbox Navigation SDK v3 (lane guidance, speed limit, voice, day/night all built-in)
- **Both platforms** — 7 events, 19 props, full feature and API parity
- NDK 27 + 16 KB page size compliant (Android 15+)

---

## Installation

```bash
npx expo install @jacques_gordon/expo-mapbox-navigation @rnmapbox/maps
```

### 1. Setup @rnmapbox/maps first

```json
["@rnmapbox/maps", {
  "RNMapboxMapsImpl": "mapbox",
  "RNMapboxMapsVersion": "11.11.0",
  "RNMapboxMapsDownloadToken": "sk.your_secret_token"
}]
```

### 2. Add this plugin

```json
["@jacques_gordon/expo-mapbox-navigation", {
  "accessToken": "pk.your_public_token",
  "downloadsToken": "sk.your_secret_token",
  "mapboxMapsVersion": "11.11.0"
}]
```

### 3. iOS only — enable static frameworks

```json
["expo-build-properties", { "ios": { "useFrameworks": "static" } }]
```

This is required by the vendored xcframework approach (same requirement as the original `youssefhenna/expo-mapbox-navigation` this package builds on). Note that `@rnmapbox/maps` itself still forces `MapboxCommon`/`MapboxCoreMaps`/`MapboxMaps`/`Turf` specifically to dynamic linkage regardless of this setting (see its own `rnmapbox-maps.podspec`) — that's expected and unrelated.

---

## Plugin Options

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `accessToken` | ✅ | — | Public Mapbox token (`pk.*`). Used for map tiles and routing. |
| `downloadsToken` | ✅ | — | Secret Mapbox token (`sk.*`) with **Downloads:Read** scope. Same token as `RNMapboxMapsDownloadToken`. Kept for backward compatibility with app.json configs from earlier versions — as of 2.3.0 it's no longer used at app-build time (the iOS SDK is vendored as prebuilt binaries; nothing downloads from `api.mapbox.com` during your `pod install`/EAS build anymore). |
| `mapboxMapsVersion` | ✅ | `"11.11.0"` IOS fixed at **(11.14.0)** | Must exactly match `RNMapboxMapsVersion` in `@rnmapbox/maps`. **Android only** as of 2.3.0. As of 2.3.7, this genuinely drives the `com.mapbox.maps:android` and (indirectly, via `mapboxNavigationVersion`'s auto-calculation) `com.mapbox.navigationcore:*` Gradle dependency versions — previously this option was silently ignored for that purpose and those versions were hardcoded. iOS SDK version is fixed per npm package release — see [iOS Architecture](#ios-architecture). |
| `mapboxNavigationVersion` | — | auto-calculated (Android only) | **Android only** (reactivated in 2.3.7 — was deprecated/unused after 2.3.0's iOS rewrite). If set, used exactly as given for `com.mapbox.navigationcore:*` Gradle dependencies — no recalculation. If omitted, derived from `mapboxMapsVersion` via the Phase 1/Phase 2 formula (see [iOS Architecture](#ios-architecture) history) — a best-effort approximation, not a guarantee. Has no effect on iOS; the iOS SDK version is fixed by which npm package version you install. |
| `androidColorOverrides` | — | `{}` | Override Mapbox native resource colors on Android. |

---

## iOS Architecture mapboxMapsVersion = 11.14.0

### How it works (as of 2.3.0) 

Mapbox Navigation SDK v3 for iOS is distributed via Swift Package Manager only — Mapbox has not shipped CocoaPods support for it. Earlier versions of this package (2.2.x) tried to bridge SPM into CocoaPods live, at your app's `pod install` time, using the same `post_install` Ruby-hook technique `@rnmapbox/maps` uses for its own dependencies. That approach turned out to be fundamentally unreliable in practice: React Native's own SPM manager silently strips manually-added SPM package references during `pod install`, and the officially-sanctioned alternative (`spm_dependency()`) is documented to cause duplicate-symbol errors on statically-linked Expo modules.

**2.3.0 takes a different approach: the iOS SDK binaries are prebuilt and vendored directly into this npm package.** Mapbox officially publishes `MapboxNavigationCore`/`MapboxNavigationUIKit`/`MapboxDirections` (and their transitive binary dependencies) as precompiled `.xcframework` downloads via a dedicated repository, [`mapbox-navigation-ios-build-artifacts`](https://github.com/mapbox/mapbox-navigation-ios-build-artifacts). This package's maintainer fetches those once per Navigation SDK version (via [`.github/workflows/build-xcframeworks.yml`](.github/workflows/build-xcframeworks.yml) on a free GitHub-hosted macOS runner) and commits them into `ios/Frameworks/`, which the podspec vendors via `s.vendored_frameworks`.

**What this means for you:**
- No network access to `api.mapbox.com` needed during your `pod install` or EAS build.
- No SPM package resolution happens in your project for this SDK at all.
- `useFrameworks: "static"` is still required (same as the original `youssefhenna/expo-mapbox-navigation` this package builds on) — see step 3 of [Installation](#installation).
- The iOS SDK version is fixed by which version of this npm package you install (matching a specific `mapboxMapsVersion`), not something you configure per-app.

**Why `MapboxMaps`/`MapboxCommon`/`MapboxCoreMaps`/`Turf` are *not* vendored here:** `@rnmapbox/maps` already installs those via CocoaPods. Vendoring a second copy of the same libraries would cause duplicate-symbol link errors. Only the Navigation-specific frameworks that `@rnmapbox/maps` doesn't already provide are vendored by this package.

### Upgrading the vendored iOS SDK version (maintainers) — actual mapboxMapsVersion = 11.11.0

The iOS binaries are tied to a specific Navigation SDK version, matched to a specific `MapboxMaps` version (see `MAPBOX_NAV_VERSION`/`MAPBOX_MAPS_VERSION` in [`ios/fetch-xcframeworks.sh`](ios/fetch-xcframeworks.sh) — `MAPBOX_MAPS_VERSION` there is a reference-only constant, recorded so this pairing lives in one place instead of only being discoverable by re-checking Mapbox's release notes each time; it isn't used to fetch anything itself, since MapboxMaps comes from CocoaPods, not this script). To bump:

1. Confirm the target Navigation version's matching Maps version from that release's own "Packaging" changelog section at `https://github.com/mapbox/mapbox-navigation-ios/releases/tag/vX.Y.Z` — don't assume a pattern holds, verify the exact release notes every time.
2. Update `MAPBOX_NAV_VERSION` and `MAPBOX_MAPS_VERSION` at the top of [`ios/fetch-xcframeworks.sh`](ios/fetch-xcframeworks.sh).
3. Update `ExpoMapboxNavigation.podspec`'s `s.dependency 'MapboxMaps', '...'` line to the exact same Maps version — this is intentionally hardcoded here, not read from either of the constants above (see the comment directly above that line in the podspec for why).
4. Also confirm `ios/ExpoMapboxNavigationModule.swift`/`ExpoMapboxNavigationView.swift` still compile against the new SDK version's actual API — this project's own custom Swift source was built against a specific API generation once before and broke silently on a version downgrade (see the 3.1.9 changelog entry) without any warning until a real archive build. There is no automated check for this yet.
5. Run the **"Build Mapbox Navigation xcframeworks"** GitHub Actions workflow with the new version tag.
6. Merge the resulting `xcframeworks/<version>` branch.
7. `npm version` + `npm publish` as usual.

---

## Usage

```tsx
import { MapboxNavigationView } from '@jacques_gordon/expo-mapbox-navigation';

export default function Navigation() {
  return (
    <MapboxNavigationView
      style={{ flex: 1 }}
      coordinates={[
        { latitude: 50.8503, longitude: 4.3517 },  // Brussels
        { latitude: 51.2194, longitude: 4.4025 },  // Antwerp
      ]}
      voiceUnits="metric"
      language="fr"
      navigationProfile="driving-traffic"
      onRoutesReady={({ nativeEvent }) =>
        console.log('Route ready:', nativeEvent.distanceMeters, 'm')
      }
      onRouteProgressChanged={({ nativeEvent }) =>
        console.log('Remaining:', nativeEvent.distanceRemaining, 'm')
      }
      onManeuverBannerPressed={({ nativeEvent }) => {
        // Open a bottom sheet showing all upcoming steps
        console.log('Steps:', nativeEvent.steps);
      }}
      onArrival={() => console.log('Arrived!')}
      onNavigationCancelled={() => console.log('Cancelled')}
      onRoutesFailed={({ nativeEvent }) =>
        console.error('Failed:', nativeEvent.message)
      }
    />
  );
}
```

---

## Props

### Navigation

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `coordinates` | `{ latitude: number; longitude: number }[]` | **required** | Waypoints. Minimum 2. |
| `waypointIndices` | `number[]` | all | Which coordinates are true waypoints (vs. route shape points). |
| `navigationProfile` | `string` | `"driving-traffic"` | `"driving-traffic"`, `"driving"`, `"walking"`, `"cycling"`. **Android**: omit the `"mapbox/"` prefix. |
| `language` | `string` | device locale | BCP-47 tag (e.g. `"fr"`, `"nl"`, `"en-US"`). |
| `voiceUnits` | `"metric" \| "imperial"` | auto by locale | Overrides automatic unit detection. |
| `excludeTypes` | `string[]` | — | Road types to avoid (e.g. `["toll", "ferry"]`). |
| `mapStyle` | `string` | Mapbox Navigation Day | Map style URL. |
| `mute` | `boolean` | `false` | Silence voice instructions. |
| `maxHeight` | `number` | — | Max vehicle height in metres. |
| `maxWidth` | `number` | — | Max vehicle width in metres. |
| `useMapMatching` | `boolean` | `false` | Use Map Matching API instead of routing. |
| `customRasterTileUrl` | `string` | — | Custom raster tile URL with `{x}/{y}/{z}`. |
| `customRasterAboveLayerId` | `string` | — | Layer ID to insert custom raster tiles above. |

### Color Customization (Android)

All color props are optional — defaults are applied when omitted.

| Prop | Default | Description |
|------|---------|-------------|
| `maneuverBackgroundColorDay` | Mapbox default | Background of the turn-by-turn instruction banner (day/light mode). Sets `ManeuverViewOptions.maneuverBackgroundColor` at runtime (Android/iOS). **Android: has no visible effect from this prop alone — must also be set as a plugin option in `app.json` for the real on-screen color to change. See the callout right below, and [priority vs. `androidColorOverrides`](#mapbox-native-colors-android-via-plugin).** |
| `maneuverBackgroundColorNight` | Mapbox default | Same, for night mode — the component switches automatically based on time of day (6am–8pm = day). **Android: same `app.json` plugin-option requirement as `maneuverBackgroundColorDay` above.** |
| `maneuverTurnIconColor` | Mapbox default | Color of the turn-direction icon inside the instruction banner. Sets `ManeuverViewOptions.turnIconManeuver` at runtime. **Android: has no visible effect from this prop alone — Mapbox's SDK has no runtime API for icon color, only a build-time XML style chain. Must also be set as a plugin option in `app.json`. See the callout right below.** |
| `navigationPuckColor` | Mapbox default | Tints Mapbox's own default location puck icon (the arrow showing your position/heading on the map — distinct from `maneuverTurnIconColor`, which is inside the banner, not on the map). Ignored if `navigationPuckImagePath` or `navigationPuck3DModelPath` is set. |
| `navigationPuckImagePath` | — | Replaces the puck icon entirely with a local image (`file://` URI or absolute path). Never tinted, even if `navigationPuckColor` is also set — avoids any risk of a tint operation failing on an arbitrary custom image. Falls back to the color/default icon if the file can't be loaded. |
| `navigationPuck3DModelPath` | — | Replaces the 2D puck with a 3D model (`.glb`/`.gltf`) — a local path, `asset://name.glb` (Android's bundled assets), or a full URL. Takes priority over both puck props above when set and valid. Falls back to the 2D puck if the model fails to load. |
| `speedLimitPosition` | `"bottomLeft"` | Position of the speed limit panel: `"bottomLeft"`, `"bottomRight"`, `"topLeft"`, or `"topRight"`. `"topRight"` uses a wider margin to clear the mute/overview/recenter buttons, which occupy that corner already — verify visually on your target devices before relying on it. |
| `showEta` | `true` | Whether the ETA/duration/distance bar is shown once navigation starts. Android only. |
| `etaBarBackgroundColor` | `"#1E2433"` | Background of the bottom ETA/duration/distance bar. |
| `etaTextColor` | `"#FFFFFF"` | Text color for ETA time and duration. |
| `iconButtonColor` | `"#1A73E8"` | Color of the mute/overview/recenter buttons (default state). |
| `iconButtonMutedColor` | `"#EA4335"` | Color of the mute button when voice is muted. |

```tsx
<MapboxNavigationView
  maneuverBackgroundColorDay="#1E2433"
  maneuverBackgroundColorNight="#0B0E14"
  maneuverTurnIconColor="#1A73E8"
  navigationPuckColor="#1A73E8"
  speedLimitPosition="bottomLeft"
  etaBarBackgroundColor="#1E2433"
  etaTextColor="#FFFFFF"
  iconButtonColor="#1A73E8"
  iconButtonMutedColor="#EA4335"
/>
```

> **Android: `maneuverBackgroundColorDay`, `maneuverBackgroundColorNight`, and `maneuverTurnIconColor` alone are not enough.**
>
> All three set a matching `ManeuverViewOptions` field at runtime — confirmed, via diagnostic logging, to correctly receive and parse the right value every time — but on Android, **none of them visibly changes anything on their own**. The maneuver banner's actual on-screen appearance (background *and* icon color) is controlled by a build-time Android resource style (`MapboxCustomManeuverStyle` / `MapboxCustomManeuverTurnIconStyle`), generated by this package's **config plugin** from its own `app.json` options — not from view props on your React component. This isn't a bug to work around: config plugins run at `expo prebuild` time, before your JS ever executes, so they have no way to read a runtime prop value — and Mapbox's SDK resolves the banner's actual colors from that compiled resource, not from the runtime API. See [Mapbox Native Colors](#mapbox-native-colors-android-via-plugin) below for the full mechanism.
>
> **You need the same three values in two places**: as `app.json` plugin options (this is what actually changes the color on screen) and, if you like, also as view props on your component (harmless — kept for iOS parity and forward-compatibility, but currently has no visible effect of its own on Android). To avoid maintaining the same hex values twice, put them in one shared file and spread it into both:
>
> ```js
> // maneuverColors.js — single source of truth
> module.exports = {
>   maneuverBackgroundColorDay: "#1E2433",
>   maneuverBackgroundColorNight: "#0B0E14",
>   maneuverTurnIconColor: "#1A73E8",
> };
> ```
>
> ```js
> // app.config.js — the JS config format, so it can require() the file above
> // (a static app.json cannot)
> const maneuverColors = require('./maneuverColors');
>
> module.exports = {
>   expo: {
>     plugins: [
>       ["@jacques_gordon/expo-mapbox-navigation", {
>         accessToken: "pk.xxx",
>         downloadsToken: "sk.xxx",
>         ...maneuverColors,
>       }],
>     ],
>   },
> };
> ```
>
> ```tsx
> // Your component — spread the same object, so there's exactly one place
> // to ever change one of these three colors
> import maneuverColors from '../maneuverColors';
>
> <MapboxNavigationView {...maneuverColors} /* ...other props */ />
> ```

> **iOS parity, as of this release:**
> - `maneuverBackgroundColorDay`/`maneuverBackgroundColorNight` — implemented via a custom `StandardDayStyle`/`StandardNightStyle` subclass pair, styling `InstructionsBannerView` through `UIAppearance` (Mapbox's own confirmed, documented v3 mechanism for this). **Read once, at the time a route is presented** — unlike Android, `NavigationOptions.styles` is a construction-time configuration, so changing these colors while navigation is already active takes effect on the *next* route, not instantly. This is an architectural difference from Android's live-updating maneuver view, not an oversight.
> - `navigationPuckColor`/`navigationPuckImagePath`/`navigationPuck3DModelPath` — implemented via `NavigationMapView.puckType` (`Puck2DConfiguration`/`Puck3DConfiguration`), the confirmed official v3 API — same precedence as Android (3D model > custom image, never tinted > color tint > default). Unlike the maneuver colors above, this **can** be updated live while navigation is active. `navigationPuckColor` tints a standard system symbol rather than Mapbox's own default icon — there's no confirmed public API for the exact built-in asset name to tint directly on iOS, unlike Android.
> - `maneuverTurnIconColor` and `speedLimitPosition` are **not implemented on iOS** and remain no-ops (stored, no effect). `maneuverTurnIconColor` would require referencing a specific internal SDK view class for the turn-direction icon that we could not confirm still exists under an unchanged name in the current v3 `MapboxNavigationUIKit` — guessing wrong would fail to compile the entire iOS build, not just silently not work, so this was left out deliberately rather than risked. `speedLimitPosition` has no confirmed repositioning API — `SpeedLimitView`'s position is fixed as part of `NavigationViewController`'s drop-in layout. Both would require building a custom top/bottom banner (see Mapbox's "Build a custom UI" guide) rather than using the drop-in `NavigationViewController`, a substantially larger undertaking than styling one already outside this package's current architecture.
> - `etaBarBackgroundColor`, `etaTextColor`, `iconButtonColor`, `iconButtonMutedColor` remain stored-only for the same reason as `maneuverTurnIconColor` — no confirmed v3 class name found for these specific sub-elements.

### Mapbox Native Colors (Android, via plugin)

Override Mapbox's built-in resource colors (route line, etc.) via `androidColorOverrides` in `app.json`:

```json
["@jacques_gordon/expo-mapbox-navigation", {
  "accessToken": "pk.xxx",
  "downloadsToken": "sk.xxx",
  "mapboxMapsVersion": "11.11.0",
  "androidColorOverrides": {
    "mapbox_primary_route_color": "#0055FF",
    "mapbox_main_maneuver_background_color": "#FF5500"
  }
}]
```

**Priority between `androidColorOverrides.mapbox_main_maneuver_background_color` and the `maneuverBackgroundColorDay` prop.** These are two independent mechanisms that both affect the maneuver banner's background — `androidColorOverrides` is a build-time Android resource override (baked into the APK/AAB at `expo prebuild`), while `maneuverBackgroundColorDay` is a runtime `ManeuverViewOptions` API call. Setting both to *different* values means only one visibly wins, which looks like the other one "isn't working." As of 3.1.2, this package resolves it with a clear, fixed priority:

1. **`androidColorOverrides.mapbox_main_maneuver_background_color`, if you set it explicitly** — always wins, exactly as you set it.
2. **Otherwise, `maneuverBackgroundColorDay`, if set** — automatically written into the same underlying resource for you. You don't need to set both; setting only `maneuverBackgroundColorDay` is enough.
3. **Otherwise, Mapbox's own default.**

In short: if you're only using one of the two, just use `maneuverBackgroundColorDay` — it's kept in sync automatically. Reach for `androidColorOverrides.mapbox_main_maneuver_background_color` directly only if you specifically need to pin an exact value regardless of what any prop says (e.g. a design system constant that shouldn't be app-configurable).

**`maneuverTurnIconColor` — plugin option only, no priority conflict.** Unlike the background color above, there's no `androidColorOverrides` key for the turn icon's color, so there's nothing to prioritize against — just set `maneuverTurnIconColor` as a plugin option and it's used directly to generate `MapboxCustomManeuverTurnIconStyle`, referenced from `MapboxCustomManeuverStyle` via `maneuverViewIconStyle`/`laneGuidanceManeuverIconStyle` (Mapbox's own official mechanism for this — see their "Change the color of maneuver turn icons" guide). Same single build-time XML style file as the background color, generated in both `res/values/` and `res/values-night/` — Mapbox ships its own `values-night/` default for this too, which would otherwise silently win in dark mode the same way it originally did for the background color.

---

## Events

| Event | Payload | Description |
|-------|---------|-------------|
| `onRoutesReady` | `{ routeCount, distanceMeters, durationSeconds }` | Fired when routes are calculated and navigation starts. |
| `onRouteProgressChanged` | `{ distanceRemaining, durationRemaining, distanceTraveled, fractionTraveled, currentStepDistanceRemaining }` | Fired on every GPS update during navigation. |
| `onArrival` | `{}` | User reached the destination. |
| `onNavigationCancelled` | `{}` | User tapped the cancel (✕) button. |
| `onNavigationFinished` | `{}` | Navigation session ended normally. |
| `onRoutesFailed` | `{ message: string }` | Route calculation failed. |
| `onManeuverBannerPressed` | `{ steps: RouteStep[] }` | Fired when user taps the instruction banner. Use to open a bottom sheet with the full steps list. |

### RouteStep type

```ts
interface RouteStep {
  instruction: string;       // "Turn left onto Main St"
  distanceMeters: number;
  durationSeconds: number;
  maneuverType: string;      // "turn", "merge", "roundabout", etc.
  maneuverModifier: string;  // "left", "right", "straight", etc.
  roadName: string;
  laneInstructions: {
    active: boolean;         // true = recommended lane
    directions: string[];    // ["straight"], ["left", "straight"]
  }[];
}
```

---

## 16 KB Page Size (Android 15+)

Google Play requires new apps/updates targeting Android 15+ (API 35+) to support 16 KB memory pages. This package addresses this via **two independent mechanisms**, covering two different Mapbox artifact groups — worth understanding both, since they're gated differently.

**Already enabled by default, no configuration needed:**
- **NDK 27** (`27.0.12077973`) — first NDK version with full 16 KB support
- **`jniLibs.useLegacyPackaging = false`** — prevents `.so` compression, enables proper alignment
- **64-bit ABI filters** (`arm64-v8a`, `x86_64`) — the 16 KB requirement applies to 64-bit only

**Mechanism 1 — `com.mapbox.maps:*`/`com.mapbox.common:*` (`Maps`/`plugin`/`module`/`extension` groups): always on, no flag.**

The config plugin's `addAndroidConfig` unconditionally substitutes these to their `-ndk27` variants via `resolutionStrategy.dependencySubstitution` (17 modules). `MAPS_VER` comes from `mapboxMapsVersion` (default `"11.11.0"`); `COMMON_VER` is auto-derived from it (Mapbox's synchronized versioning: `11.x.y` ↔ `24.x.y`, same minor/patch — verified against Mapbox's own compatibility table). This has been on since before `androidUseNdk27` existed and isn't gated by it — it runs every time regardless.

**Mechanism 2 — `com.mapbox.navigationcore:*`: opt-in — `androidUseNdk27`.**

Mapbox publishes a separate `-ndk27` artifact variant of each `com.mapbox.navigationcore:*` dependency (the one this package itself declares in `android/build.gradle`) — but **only starting from Navigation SDK 3.11.0** (this package defaults to `3.8.1`, which predates it; confirmed from [Mapbox's own changelog](https://raw.githubusercontent.com/mapbox/mapbox-navigation-android/master/CHANGELOG.md)). Since switching to a nonexistent `-ndk27` artifact fails the build outright (`Could not resolve`), this one is opt-in rather than default:

```json
["@jacques_gordon/expo-mapbox-navigation", {
  "accessToken": "pk.xxx",
  "downloadsToken": "sk.xxx",
  "mapboxMapsVersion": "11.14.0",
  "mapboxNavigationVersion": "3.11.0",
  "androidUseNdk27": true
}]
```

Requirements when enabling this:
- Pass **both** `mapboxMapsVersion` and `mapboxNavigationVersion` explicitly — don't rely on this package's Phase 1/Phase 2 auto-calculation for a version pair you haven't verified. Mapbox's own docs note that pairing Navigation/Maps versions incorrectly is easy to get wrong for anything below Navigation 3.16.0.
- Use a `mapboxNavigationVersion` of `3.11.0` or later (and a correspondingly compatible `mapboxMapsVersion` — check [Mapbox's Navigation SDK release notes](https://github.com/mapbox/mapbox-navigation-android/blob/main/CHANGELOG.md) for the exact tested pairing at your target version).

If you enable `androidUseNdk27` with an unsupported version pair, Gradle will fail clearly with `Could not resolve com.mapbox.navigationcore:...-ndk27:...` — a hard failure, not a silent fallback, so you'll know immediately if the pairing is wrong.

**In short**: leaving everything at defaults gets you Mechanism 1 automatically (Maps/Common covered). Google Play flagging only `libnavigator-android.so` specifically means Mechanism 2 is what you still need — set `androidUseNdk27: true` with a verified `>= 3.11.0` pairing.

See [Android's 16 KB page size guide](https://developer.android.com/guide/practices/page-sizes).

---

## Changelog

### 3.3.2
**⚠️ Still not confirmed fixed on a real device. Adds a `post_install` safety net + real diagnostics for the `MapboxMaps` vendored pod target, after a real `pod install` log showed the Podfile override from 3.3.0 working correctly at the resolution level, but `@rnmapbox/maps` separately touching a pod target literally named "MapboxMaps" afterward.**

- **What a real build showed**: the Podfile override from 3.3.0 does work as intended — a real `pod install` log confirmed `Fetching podspec for 'MapboxMaps' from .../ios/MapboxMaps.podspec` and `Installing MapboxMaps (11.20.0)`, i.e. CocoaPods correctly used this package's local override, not trunk. But the very next lines of that same log showed `* [RNMapbox] Changed MapboxMaps to dynamic framework` — `@rnmapbox/maps` separately touches a pod target by that name regardless of where it came from — followed immediately by a real compile error: `no such module 'MapboxMaps'` in `@rnmapbox/maps`'s own `Array+asExpressions.swift`.
- **Working hypothesis, not confirmed**: `@rnmapbox/maps` most likely registers this via CocoaPods' own `Pod::HooksManager` plugin API from inside its own podspec (no visible Podfile-level `post_install` block triggers it), and that logic likely assumes a normal, source-compiled MapboxMaps pod target (the usual trunk case) — this package's own `ios/MapboxMaps.podspec` is vendored-frameworks-only (no source to compile at all), so the same adjustment applied to it may not behave as intended and could plausibly disrupt how the module gets exposed to dependent targets like `@rnmapbox/maps`'s own Swift sources.
- **What was done**: a second Podfile injection in `plugin/src/index.js`, a `post_install do |installer| ... end` block appended to the generated Podfile. Podfile-level `post_install` callbacks run after any pod-specific `Pod::HooksManager`-registered hooks (like `@rnmapbox/maps`'s own), so this reliably runs last. It re-asserts `DEFINES_MODULE = 'YES'` on the `MapboxMaps` pod target's build configurations, and — since simply guessing again didn't work last time — also **prints that target's actual resulting build settings** (`MACH_O_TYPE`, `DEFINES_MODULE`, `FRAMEWORK_SEARCH_PATHS`, `BUILD_LIBRARY_FOR_DISTRIBUTION`) for every configuration, so the next attempt has real data instead of another blind patch if this doesn't fully fix it.
- **Not yet verified** — untested whether `DEFINES_MODULE` alone is the missing piece; the diagnostic output is there specifically in case it isn't.

### 3.3.1
**Three small corrections to 3.3.0's new `ios/MapboxMaps.podspec`, found before any build was attempted:**

- `s.vendored_frameworks`: changed from `File.join(__dir__, 'Frameworks/MapboxMaps.xcframework')` (an absolute path) to `'Frameworks/MapboxMaps.xcframework'` (relative to the podspec's own location, as CocoaPods requires for this field).
- `s.summary`: shortened to 101 characters (CocoaPods enforces a practical limit here); the original, more detailed text moved to `s.description` instead, which has no strict length limit.
- The GitHub Actions workflow name referenced in this podspec's own warning message updated to match its actual current name.

### 3.3.0
**⚠️ INCOMPLETE — `ios/Frameworks/` is currently EMPTY, cannot be built or run until the "Build Mapbox Navigation xcframeworks" GitHub Actions workflow is run. The most significant milestone in this entire investigation: the actual, Mapbox-confirmed root cause of the recurring DYLD `Symbol not found: GestureType.singleTap` launch crash, and a real structural fix — not another version pairing guess.**

- **The confirmed root cause**, directly from a Mapbox engineer on their own issue tracker ([mapbox/mapbox-maps-ios#1669](https://github.com/mapbox/mapbox-maps-ios/issues/1669)), not this project's own theory: *"Because the CocoaPods and SwiftPM \[source\] versions of the framework are provided by source and the project that builds it does not set `BUILD_LIBRARY_FOR_DISTRIBUTION` to YES, some symbols are stripped from the binary... In the scenario where a third party compiled framework would also depend on the MapboxMaps module, the build would fail (missing symbols)... a runtime error if the framework is dynamic."* This package's own vendored `MapboxNavigationCore.xcframework` — however it gets built, whether downloaded precompiled from Mapbox or compiled from source via Scipio — is exactly that "third party compiled framework." It depends on MapboxMaps' full witness tables (library evolution). `@rnmapbox/maps` installs MapboxMaps via CocoaPods, which is missing those symbols by design — a mismatch that has nothing to do with SDK version pairing, and nothing to do with how `MapboxNavigationCore` itself gets compiled. This was proven empirically over several versions in this investigation, most conclusively by comparing real crash-report binary UUIDs: even a from-source Scipio build of `MapboxNavigationCore` (3.2.0), linked against the exact same MapboxMaps version CocoaPods resolved, crashed with the identical missing symbol.
- **The fix**: Mapbox publishes a real, official, separate binary distribution specifically for this — [`mapbox-maps-ios-binary`](https://github.com/mapbox/mapbox-maps-ios-binary) — built WITH `BUILD_LIBRARY_FOR_DISTRIBUTION=YES`, the same way `mapbox-navigation-ios-build-artifacts` already builds `MapboxNavigationCore` itself. Binary releases are available starting from MapboxMaps v11.20.0 — which happens to be exactly the version Navigation SDK v3.20.0 requires (confirmed directly from `mapbox-navigation-ios`'s own `CHANGELOG.md`, not assumed), making this pairing the natural, most-conservative starting point for this fix.
  - `ios/fetch-xcframeworks.sh` rewritten: still downloads `MapboxNavigationCore`/`UIKit`/`Directions`/`_MapboxNavigationHelpers`/`_MapboxNavigationLocalization`/`MapboxNavigationNative` from `mapbox-navigation-ios-build-artifacts` as before (now at v3.20.0), and now ALSO downloads `MapboxMaps.xcframework` itself from `mapbox-maps-ios-binary` (v11.20.0).
  - `ExpoMapboxNavigation.podspec`: `s.dependency 'MapboxMaps'` removed entirely — MapboxMaps is vendored now, alongside the Navigation-specific frameworks. `MapboxCommon`/`MapboxCoreMaps`/`Turf` remain CocoaPods dependencies, unchanged.
  - **New: `ios/MapboxMaps.podspec`** — a small, standalone local podspec, deliberately named `MapboxMaps` (matching the real CocoaPods trunk pod), vendoring the same binary. Not autolinked by Expo/React Native — referenced explicitly via a new Podfile override.
  - **New: a `withDangerousMod` Podfile override in `plugin/src/index.js`** — inserts `pod 'MapboxMaps', :podspec => '../node_modules/@jacques_gordon/expo-mapbox-navigation/ios/MapboxMaps.podspec'` right before `use_expo_modules!`/`use_native_modules!` in the consuming app's generated Podfile. CocoaPods resolves exactly one version/source per pod name across an entire Podfile — this explicit declaration takes precedence over what `@rnmapbox/maps`' own `s.dependency 'MapboxMaps'` would otherwise pull from trunk, so `@rnmapbox/maps` ends up linking our vendored copy too. One `MapboxMaps` binary in the final app, not two — avoiding a duplicate-symbol problem that vendoring alone (without this override) would have reintroduced.
- **`.github/workflows/build-xcframeworks.yml`** updated: default version bumped to `3.20.0`, timeout increased (now fetches from two separate Mapbox repos, each with its own SPM resolution), and the frameworks-verification step extended to check for `MapboxMaps.xcframework` specifically.
- **`ios/Frameworks/` emptied** — the previous contents (3.11.0-era Scipio-built binaries, no `MapboxMaps.xcframework`) are stale and incompatible with this version's architecture. **This package cannot be built or published as a working release until the GitHub Actions workflow is actually run** and its output committed. No network access was available to run this from wherever this version was prepared.
- **Not yet verified end-to-end on a real device** — the root cause is now confirmed by Mapbox's own engineering team, not a guess, and the fix follows their own stated resolution path directly. This is the strongest basis for confidence reached in this entire investigation, but it is still unconfirmed until `ios/Frameworks/` is populated and a real device build is tested.

### 3.2.0
**Bumped from 3.1.12 → 3.2.0 to mark this as a real milestone, not another patch: the first version in this entire investigation combining this project's own complete feature set (never broken, unlike the 3.8.0 attempt) with `MapboxNavigationCore` built from source via Scipio against the exact same MapboxMaps version CocoaPods will separately resolve (11.14.0) — a real, successful, artifact-producing build behind it, not a downloaded precompiled binary from a separate channel. `ios/Frameworks/` populated with the real output of a successful GitHub Actions run of `ios/fetch-xcframeworks-scipio.sh` (build #19, 29m 27s, pushed to `xcframeworks-scipio/3.11.0`).**

- **What was fixed to get here, in order (see prior entries for full detail on each)**: (1) `Package.swift` parse failure — traced to `swift-custom-dump`'s tools-version requirement exceeding Scipio's own embedded SwiftPM, via youssefhenna's test-dependency-free manifest technique. (2) `xcbuild` missing on Xcode 16.3/16.4 on GitHub-hosted runners — pinned to Xcode 16.2. (3) Missing iOS Simulator runtime causing an asset-catalog failure — fixed with `xcodebuild -downloadPlatform iOS`, later found to itself be flakey on GitHub's runners (~40% failure rate, a documented, unrelated runner-image issue) and wrapped in a retry loop. (4) `_MapboxNavigationHelpers` missing when building `_MapboxNavigationLocalization` — a genuine missing dependency edge in this project's own hand-written manifest, confirmed and fixed against the real official `Package.swift`. (5) **The actual root cause of the final blocker**: `_OBJC_CLASS_$_MBNN*` undefined symbols linking `MapboxNavigationCore` against `MapboxNavigationNative` — traced to a real, externally-documented SwiftPM limitation (matching [tuist/tuist#8056](https://github.com/tuist/tuist/issues/8056) exactly: transitive dependencies of binary targets aren't reliably resolved when the consuming target itself becomes a binary product) affecting the official `Package.swift`'s `.package()`-based declaration of `mapbox-navigation-native-ios` at this version. Fixed by reverting to youssefhenna's own original template structure — a manually-pinned `.binaryTarget` with an explicit checksum for `MapboxNavigationNative` instead — automating their documented manual "run once, fails, discover the real checksum, update, don't rerun" process into the script itself.
- **A genuine, unexpected bonus discovered by auditing the real output, not assumed**: every `.swiftinterface` in these Scipio-built frameworks already declares `-target arm64-apple-ios15.1` directly — confirmed across all 5 built frameworks, both device and simulator slices. Unlike Mapbox's own officially precompiled `mapbox-navigation-ios-build-artifacts` binaries (which hardcode `-target arm64-apple-ios14.0`, requiring this project's own `14.0`→`15.1` `.swiftinterface` patch step), Scipio's from-source build correctly computed the effective 15.1 deployment target from the real dependency graph on its own. The patch step in `fetch-xcframeworks-scipio.sh` still ran as a no-op safety net (0 files patched) rather than being removed, in case a future version pairing doesn't have this same property.
- **What was done**: `ios/Frameworks/` replaced with the real output of a successful GitHub Actions run (build #19, 29m 27s, pushed to `xcframeworks-scipio/3.11.0`) — `MapboxNavigationCore`, `MapboxNavigationUIKit`, `MapboxDirections`, `_MapboxNavigationHelpers`, `_MapboxNavigationLocalization` (all reporting `1.0` as their `CFBundleShortVersionString` — Scipio doesn't fill in the real SDK version tag, a cosmetic-only detail, not a version mismatch), and `MapboxNavigationNative` (correctly reporting the real `324.14.0`, confirming the checksum-discovery step worked correctly). All confirmed dynamic frameworks (`MH_DYLIB`), both `ios-arm64` (device) and `ios-arm64_x86_64-simulator` slices present for every framework.
- **Not yet verified end-to-end on a real device** — this is the most complete, most carefully-audited configuration reached in this entire investigation: this project's own full feature set, building `MapboxNavigationCore` from source against the exact `mapbox-maps-ios` SPM tag CocoaPods will separately resolve (11.14.0), with a real, successful, artifact-producing build behind it. If the original DYLD `GestureType.singleTap` crash persists even here, that would be strong evidence the cause is specific to something in Navio's own build environment (its Podfile, its ~184-pod dependency graph, or EAS Build itself) rather than anything reachable from within this package.

### 3.1.11
**⚠️ INCOMPLETE — `ios/Frameworks/` is currently EMPTY, this cannot be built or run yet. Reverts iOS to this project's own full custom implementation (all color/puck/ETA features, Navigation 3.11.0 / MapboxMaps 11.14.0), abandoning the 3.1.9/3.1.10 approach of adopting youssefhenna/expo-mapbox-navigation's Swift source and binaries wholesale — while adopting their actual xcframework-building *technique* (a minimal, hand-written `Package.swift` + Scipio) for a version pairing they haven't themselves verified. Android completely untouched.**

- **Why revert away from 3.1.9/3.1.10**: those versions vendored youssefhenna's own actual Swift source and binaries (Navigation 3.8.0 / MapboxMaps 11.11.0) to test whether the recurring DYLD crash was fixable by matching their exact proven configuration — it wasn't (confirmed via a real archive build log): this project's own `ExpoMapboxNavigationView.swift` (grown over many sessions to add color customization, custom pucks, ETA bar, etc.) targets Navigation SDK API surface that simply doesn't exist at 3.8.0 (`NavigationOptions`, `NavigationViewController`'s `navigationView` property, `puckType`/`Puck2DConfiguration`/`Puck3DConfiguration`), so 3.8.0 can never compile this project's own features without rewriting them against an older, more limited API. Since 3.11.0/11.14.0 is what this project's own Swift source was actually written against, staying there is necessary to keep every feature working, at the cost of returning to a version pairing this project has already seen the original DYLD crash on twice.
- **What was done**:
  - `ios/ExpoMapboxNavigationModule.swift` and `ios/ExpoMapboxNavigationView.swift` restored from `ios/_legacy_custom_swift_backup/*.bak` — this project's own full implementation, all props/events intact, completely unaffected by any of the youssefhenna-source experiment.
  - `ExpoMapboxNavigation.podspec` restored to its pre-3.1.7 structure: `s.platforms = '15.1'`, `IPHONEOS_DEPLOYMENT_TARGET`/`ENABLE_TESTABILITY` overrides back, `MapboxCommon`/`MapboxCoreMaps` explicit dependencies back, `s.dependency 'MapboxMaps', '11.14.0'`.
  - **`ios/fetch-xcframeworks-scipio.sh` rewritten**, this time following youssefhenna/expo-mapbox-navigation's *actual* documented technique (confirmed by reading their own README's "Getting the `.xcframework` files" section directly, not inferred) — a minimal, hand-written `Package.swift` that **replaces** the official one entirely (reusing the cloned repo's `Sources/` directories, just with a slimmer manifest), rather than this project's earlier approach of patching the full official manifest to strip out its test-only dependency chain. This sidesteps the Scipio/Swift-tools-version incompatibility that patching had to work around, and avoids needing to artificially declare `MapboxDirections` as a product to coax Scipio into building it (youssefhenna's own output includes `MapboxDirections.xcframework` despite never declaring it as a product either — untested but plausible that Scipio builds an xcframework per reachable target, not just requested products).
  - The minimal `Package.swift` this script generates was hand-written by directly reading the **real, official** `Package.swift` at tag `v3.11.0` for its exact target dependency graph — notably that `MapboxNavigationCore` depends on `_MapboxNavigationLocalization` at this version (new since youssefhenna's own older 3.8.0-era template) and that `mapbox-navigation-native-ios` is a normal SPM package dependency at this version (not a manually-pinned `.binaryTarget` with an explicit checksum, the way youssefhenna's older 3.8.0-era template needed).
  - **`ios/Frameworks/` emptied entirely** (previously held youssefhenna's own 3.8.0/11.11.0 binaries from 3.1.10, incompatible with this version's 11.14.0 pin) — this package **cannot be built or published as a working release until the "Build Mapbox Navigation xcframeworks (Scipio)" GitHub Actions workflow is actually run** (with `mapbox_nav_version: 3.11.0`) and its output branch merged. No network access or macOS/Xcode toolchain was available to run this from wherever this version was prepared.
- **Known, documented gaps in the rewritten script, untested end-to-end**: (1) `MapboxNavigationNative.xcframework` is not yet copied — it should resolve as its own binary dependency via normal SPM resolution now that it's declared as a regular package dependency rather than a manual binary target, but exactly where Scipio places it hasn't been confirmed on a real run. (2) Scipio's actual output directory name/path is asserted, not confirmed, for this specific minimal manifest. (3) Whether Scipio's own build already produces a correct 15.1+ deployment target (making the 14.0→15.1 `.swiftinterface` patch step a harmless no-op) or still needs it is unconfirmed.
- **This version is a preparation step, not a usable release** — do not publish/install until `ios/Frameworks/` is populated with real, working binaries from an actual workflow run.

### 3.1.10
**Replaces this package's own vendored iOS xcframeworks (previously fetched from `mapbox-navigation-ios-build-artifacts` via `ios/fetch-xcframeworks.sh`) with youssefhenna/expo-mapbox-navigation's own actual, real binaries — a full, direct copy from their repository, not just matching source/podspec structure as in 3.1.9. Corrects an invalidated theory from an earlier same-day attempt — see below. Android completely untouched.**

- **Why**: by 3.1.9, this project had matched youssefhenna's actual Swift source, podspec structure, and their documented 3.8.0/11.11.0 SDK/Maps pairing — and the exact same DYLD `Symbol not found: GestureType.singleTap` launch crash still occurred, confirmed via crash reports with differing binary UUIDs each time (genuinely fresh builds, not a stale cache). The one thing 3.1.9 could *not* replicate was youssefhenna's own actual xcframework binaries — this project was still fetching its own copies from `mapbox-navigation-ios-build-artifacts` (Mapbox's official precompiled distribution), while youssefhenna builds theirs directly from source via Scipio (see the 3.1.7/3.1.8 changelog entry on the paused Scipio experiment). Even at a matching version *number*, these are two different build pipelines producing two different physical binaries — the same "two channels, not guaranteed ABI-identical" concern this whole investigation kept circling back to, just never actually eliminated until now.
- **What was done**: `ios/Frameworks/` replaced entirely with youssefhenna/expo-mapbox-navigation's own actual `.xcframework` files (`MapboxDirections`, `MapboxNavigationCore`, `MapboxNavigationNative`, `MapboxNavigationUIKit`, `_MapboxNavigationHelpers` — note: no `_MapboxNavigationLocalization`, which youssefhenna's own build doesn't produce; harmless, `s.vendored_frameworks` discovers whatever's actually present via `Dir.glob` rather than a hardcoded list). This project's own previous `ios/Frameworks/` (fetched via `mapbox-navigation-ios-build-artifacts`, confirmed at Navigation 3.8.0) is backed up at `ios/_legacy_custom_swift_backup/Frameworks_ours_mapbox-build-artifacts_3.8.0/` — not deleted, kept as a reference point in case this doesn't resolve the crash either and the two build pipelines need direct binary comparison.
- **Corrects an invalidated theory from earlier the same day**: an intermediate attempt (briefly also called "3.1.10" before this) removed `s.static_framework = true` from the podspec, theorizing a static/dynamic linkage mismatch between this pod's own compiled Swift and `@rnmapbox/maps`'s forced-dynamic `MapboxMaps`. That theory didn't hold up: a direct Mach-O header check of the vendored `MapboxNavigationCore` binary confirmed it's already `MH_DYLIB` (a dynamic library) regardless of this podspec setting, which only affects how CocoaPods packages this pod's own small amount of Swift source, not the vendored binary's own fixed linkage. `s.static_framework = true` is restored, matching youssefhenna's own podspec exactly — the most faithful reproduction of their known-working configuration now possible, since this version also vendors their actual binaries directly.
- **Not yet verified on a real device** — this is now the closest possible reproduction of youssefhenna's actual, proven-working iOS configuration: their real Swift source (since 3.1.9), their real podspec structure, and now their real compiled binaries too, all three together for the first time. If the crash persists even here, it would strongly suggest the cause is specific to something in this project's own build environment (Navio's Podfile, its ~184-pod dependency graph, or EAS Build itself) rather than anything reachable from within this package.

### 3.1.9
**iOS: reverted `ios/ExpoMapboxNavigationModule.swift` and `ios/ExpoMapboxNavigationView.swift` wholesale to youssefhenna/expo-mapbox-navigation's own actual, proven-in-production files — after the 3.8.0/11.11.0 downgrade in 3.1.7/3.1.8 (see below) surfaced a real compile-time API mismatch, not just the DYLD launch crash this whole investigation started from. Android completely untouched. This is a deliberate, temporary trade-off — see "What this costs, for now" below.**

- **Why**: downgrading the vendored SDK to Navigation 3.8.0 / MapboxMaps 11.11.0 (3.1.7/3.1.8) — youssefhenna's own actually-tested pairing — surfaced a *different*, blocking problem: this project's own `ExpoMapboxNavigationView.swift` (grown over many sessions to add color customization, custom pucks, ETA bar, ETA/speed-limit styling, etc.) was written against a newer Navigation SDK API surface than 3.8.0 actually has. Confirmed directly from a real archive build log — concrete compile errors, not a guess:
  - `cannot find 'NavigationOptions' in scope`
  - `incorrect argument labels in call` for `NavigationViewController(...)`
  - `value of type 'NavigationViewController' has no member 'navigationView'` (×3)
  - `cannot infer contextual base in reference to member 'puck3D'`/`'puck2D'` (×4)
  
  In short: this project's own Swift source and the 3.8.0 binaries are from two different eras of the same SDK's API. Since youssefhenna's own `Module.swift`/`View.swift` were written *for* that same 3.8.0-era API and are known to actually build and run (this package's origin project), replacing ours with theirs — rather than trying to patch our newer code down to an older API piecemeal, blind, overnight — was the only reliable path to a working build by morning.
- **What was done**: `ios/ExpoMapboxNavigationModule.swift` and `ios/ExpoMapboxNavigationView.swift` replaced with youssefhenna/expo-mapbox-navigation's actual files, verbatim. The podspec was already aligned with youssefhenna's structure in 3.1.7/3.1.8 (see that entry below) and needed no further changes for this. `MapboxMaps` stays pinned at the exact `'11.11.0'` this project chose deliberately (see 3.1.7/3.1.8's reasoning) rather than youssefhenna's `ENV[...]`-based approach.
- **This project's own previous custom Swift files are backed up, not deleted**: `ios/_legacy_custom_swift_backup/ExpoMapboxNavigationModule.swift.bak`, `ExpoMapboxNavigationView.swift.bak`, and the podspec state right before this change (`ExpoMapboxNavigation.podspec.bak`) — kept specifically so every custom feature below can be re-ported feature-by-feature onto the youssefhenna-based file, once a real device build confirms the 3.8.0/11.11.0 pairing actually resolves the original DYLD crash. Not committed as active source, purely a reference for future work.
- **What this costs, for now — temporarily inactive on iOS only, Android fully unaffected**: this project's own color-customization and puck/ETA features stop having any effect on iOS until re-ported (silently no-op, not a crash — the shared TypeScript prop types in `src/index.tsx` were deliberately left untouched, so nothing breaks at the JS/TS layer on either platform):
  - `maneuverBackgroundColorDay`/`Night`, `maneuverTurnIconColor`
  - `navigationPuckColor`/`navigationPuckImagePath`/`navigationPuck3DModelPath`
  - `etaBarBackgroundColor`/`etaTextColor`/`iconButtonColor`/`iconButtonMutedColor`
  - `voiceUnits`, `customRasterAboveLayerId` (youssefhenna's file has a differently-named/shaped equivalent — `placeCustomRasterLayerAbove` — not yet reconciled)
  - The `onManeuverBannerPressed` event and its full-route-steps payload
  
  Core navigation still works on iOS: `coordinates`, `waypointIndices`, `mapStyle`, and `mute` use identical prop names in both this project's TypeScript layer and youssefhenna's native implementation, so they continue to work unmodified.
- **Not yet verified on a real device** — this fixes a real compile-time blocker (confirmed via build log) but the ORIGINAL DYLD launch crash investigation this whole effort started from is still not confirmed fixed by a real device run. This is the most promising configuration reached so far: youssefhenna's own actual, working Swift source, his podspec structure, and his actually-tested SDK/Maps pairing, all three matched together for the first time in this project's history — rather than a partial mix of his structure with this project's own newer, incompatible source.
- Repacked and published as **3.1.9** (skipping 3.1.7/3.1.8, which were prepared but superseded by this same night's work before being published — see below for what they contain, since their content is fully carried forward into this version).

### 3.1.7 / 3.1.8 (superseded by 3.1.9 above before publishing — kept here for the historical record)
**Downgrades the vendored iOS SDK to Navigation 3.8.0 / MapboxMaps 11.11.0 — youssefhenna/expo-mapbox-navigation's own actually-tested pairing — and aligns the podspec's own structure much more closely with that known-working baseline, removing several unverified additions this project made to its own theories. Also documents a PAUSED, EXPERIMENTAL alternative build path (Scipio, from source) for the same crash.**

- **Podspec aligned closely with youssefhenna/expo-mapbox-navigation's own, confirmed-working podspec**, after a direct line-by-line comparison surfaced several differences this project had accumulated from its own unverified theories:
  - `s.platforms` reverted from `'15.1'` to `'13.4'` (youssefhenna's value) — the `'15.1'`/`IPHONEOS_DEPLOYMENT_TARGET` override was added earlier in this investigation based on a real build error, but its actual root cause was never conclusively pinned down; it may have been specific to whichever xcframework build was in place at the time, not a structural requirement of this podspec. **Revisit this specifically if a deployment-target-related build error reappears** — it's a real, previously-confirmed error class, just not one this exact combination (youssefhenna's Swift source + this pairing) has actually been tested against yet.
  - Removed `s.dependency 'MapboxCommon'`/`'MapboxCoreMaps'` — added on an unverified theory that this project's vendored frameworks' private Swift interfaces needed them declared here; youssefhenna's podspec declares only `MapboxMaps` and `Turf` and builds successfully.
  - Removed the `ENABLE_TESTABILITY`/`s.user_target_xcconfig` override and the `IPHONEOS_DEPLOYMENT_TARGET` pod_target_xcconfig override — neither exists in youssefhenna's podspec.
  - Added `OTHER_SWIFT_FLAGS => '$(inherited)'`, `s.preserve_paths`, and a `'~> 4.0.0'` constraint on the `Turf` dependency — all matching youssefhenna's podspec exactly (the `Turf` constraint was missed in an earlier pass of this same alignment and caught afterward).
  - `MapboxMaps` stays pinned to an exact version (`'11.11.0'`) rather than youssefhenna's `ENV[...]`-based approach — a deliberate difference, not an oversight (see the comment above that line in the podspec for the full reasoning: this project's own npm package version should fully determine the iOS Maps pairing, not a value a consumer could set inconsistently).
- **Downgraded vendored Navigation SDK from 3.11.0 to 3.8.0, paired with MapboxMaps 11.11.0 (was 11.14.0).** This package originates as a fork of [`youssefhenna/expo-mapbox-navigation`](https://github.com/YoussefHenna/expo-mapbox-navigation), whose own docs state it was *"developed and tested for Mapbox Maps version 11.11.0"* — a pairing that's actually been run in practice, not just documented as compatible. Confirmed directly from a real screenshot of `mapbox-navigation-ios`'s own `CHANGELOG.md`: MapboxMaps v11.11.0 is required by Navigation Core **3.8.0** specifically (paired with MapboxNavigationNative v324.0.0) — notably **not** 3.8.2, the version this project was actually on when the very first crash in this whole investigation occurred. It's likely 3.8.2 (a later patch release) bumped its own required MapboxMaps version past 11.11.0 without this being re-verified at the time.
  - `ios/fetch-xcframeworks.sh`'s `MAPBOX_NAV_VERSION`/`MAPBOX_MAPS_VERSION` and `ExpoMapboxNavigation.podspec`'s `s.dependency 'MapboxMaps'` all updated together to `3.8.0`/`11.11.0`.
  - **Action required in the consuming app**: `RNMapboxMapsVersion` in `@rnmapbox/maps`'s config must be updated to exactly `"11.11.0"` to match.
- **⏸️ PAUSED, EXPERIMENTAL: `ios/fetch-xcframeworks-scipio.sh` and `.github/workflows/build-xcframeworks-scipio.yml`, an alternative to the proven `fetch-xcframeworks.sh`/`build-xcframeworks.yml` pair (left completely untouched and unaffected by any of this).** Investigated the same DYLD `Symbol not found: GestureType.singleTap` launch crash from a different angle: instead of downloading Mapbox's own precompiled `mapbox-navigation-ios-build-artifacts` binaries, build `MapboxNavigationCore`/`MapboxNavigationUIKit`/etc. from SOURCE via [Scipio](https://github.com/giginet/Scipio), following the approach youssefhenna/expo-mapbox-navigation uses, based on a Mapbox engineer's guidance on [mapbox/mapbox-navigation-ios#4703](https://github.com/mapbox/mapbox-navigation-ios/issues/4703).
  - **Real progress made, each confirmed against actual CI logs, not guessed**: (1) `Package.swift` parsing failure inside Scipio (`Invalid package... the data couldn't be read`) — traced to a real, explicit error (`swift-custom-dump` requiring a newer Swift tools-version than Scipio's own embedded SwiftPM supports) and fixed by stripping mapbox-navigation-ios's test-only dependency chain from a local, uncommitted copy of the manifest. (2) `xcbuild` missing entirely on Xcode 16.3/16.4 on the CI runner — fixed by pinning to Xcode 16.2. (3) A missing iOS Simulator runtime causing an asset-catalog build failure inside `mapbox-maps-ios`'s own source — fixed with an explicit `xcodebuild -downloadPlatform iOS` step.
  - **Paused at**: a linker failure specifically on `MapboxNavigationCore` — dozens of `Undefined symbol: _OBJC_CLASS_$_MBNN*` errors, meaning `MapboxNavigationNative`'s Objective-C symbols aren't visible at link time inside Scipio's build graph.
  - **Decision**: paused here rather than continuing to iterate blind. The scripts/workflow are left in place, working correctly up to this exact point, as a documented starting point for whoever picks this back up — not deleted, not merged into the default build path.
- **SUPERSEDED, kept here for the historical record**: an earlier attempt pinned `s.dependency 'MapboxMaps', '11.14.0'` while keeping Navigation at 3.11.0 and all of this project's own accumulated podspec additions — fixed the theoretical risk of CocoaPods resolving an arbitrary MapboxMaps version, but did NOT fix the actual crash, since 3.11.0/11.14.0 crashed identically on a genuinely fresh build even with this constraint in place.

### 3.1.6
**A real production iOS launch crash fixed (confirmed binary incompatibility, from an actual crash report), plus the `maneuverTurnIconColor` doc/implementation gap closed and an Android 16 edge-to-edge fix for the ETA bar.**

- **Fixed: app crashed at launch on iOS, every time, with no navigation UI ever appearing.** Confirmed directly from a real device crash report (not a guess): a `DYLD`/`Symbol missing` termination —
  ```
  Symbol not found: _$s10MapboxMaps11GestureTypeO9singleTapyA2CmFWC
  Referenced from: .../MapboxNavigationCore.framework
  Expected in:      .../MapboxMaps.framework
  ```
  Root cause: the vendored `MapboxNavigationCore` xcframework (Navigation SDK **3.8.2**) was compiled against a newer/different `MapboxMaps` API surface than what was actually linked at runtime (`MapboxMaps` **11.11.0**, installed via `@rnmapbox/maps`'s `RNMapboxMapsVersion`) — a hard binary incompatibility, not something any podspec/Podfile/project setting can paper over.
  - **Fix**: vendored xcframeworks rebuilt at Navigation SDK **3.11.0**, paired with `RNMapboxMapsVersion: "11.14.0"`. This exact pairing is confirmed directly from Mapbox's own [v3.11.0 release notes](https://github.com/mapbox/mapbox-navigation-ios/releases/tag/v3.11.0) ("Packaging: MapboxNavigationCore now requires MapboxMaps v11.14.0, MapboxNavigationCore now requires MapboxNavigationNative v324.14.0") — not an assumed-compatible version, the literal one Mapbox states is required. Verified against the actual vendored binaries' own `Info.plist` (`MapboxNavigationCore`/`MapboxNavigationUIKit`/`MapboxDirections`/`_MapboxNavigationHelpers` all report `3.11.0`; `MapboxNavigationNative` reports `324.14.0`) — both match exactly.
  - **`ios/fetch-xcframeworks.sh`'s own default `MAPBOX_NAV_VERSION` updated from `3.8.2` to `3.11.0`**, so it now matches what's actually vendored/committed. Previously, re-running this script without an explicit override would have silently regressed back to the crashing `3.8.2`/`11.11.0` pairing.
  - **Maintainer note, going forward**: this package's own `s.platforms`/`IPHONEOS_DEPLOYMENT_TARGET` (currently `15.1`) were not affected by this specific bump — no change in MapboxMaps 11.14.0's own minimum deployment target was found in its release notes. Any future Navigation SDK version bump must re-check both the exact required `MapboxMaps` pairing (from that release's own "Packaging" changelog section) **and** whether that paired `MapboxMaps` version raised its own minimum iOS deployment target.
- **Fixed: `maneuverTurnIconColor` had no actual effect on Android, even when set correctly.** The [Color Customization](#color-customization-android) and [Mapbox Native Colors](#mapbox-native-colors-android-via-plugin) sections already documented this prop needing to be set as a plugin option in `app.json` (not just as a view prop) — but the config plugin never actually read or acted on it: `maneuverTurnIconColor` was never destructured from the plugin's own `options` parameter, so no `MapboxCustomManeuverTurnIconStyle`/`maneuverViewIconStyle` was ever generated, regardless of what was set anywhere. Confirmed via real-device diagnostic logging (`MapboxCustomManeuverStyle lookup returned resId=0`) before fixing — same class of bug as the `maneuverBackgroundColorDay` destructuring bug in 3.1.2's history.
  - **Fix**: the plugin now reads `maneuverTurnIconColor` from its options and generates the full style chain confirmed against Mapbox's own official Android Navigation SDK guide ("Change the color of maneuver turn icons") — `MapboxCustomManeuverTurnIconStyle` (parent `MapboxStyleTurnIconManeuver`), referenced from `MapboxCustomManeuverStyle` via `maneuverViewIconStyle`/`laneGuidanceManeuverIconStyle`. Generated in both `res/values/` and `res/values-night/`, same reasoning as the background color fix in 3.1.5 (Mapbox ships its own `values-night/` default for this too). No separate day/night variant of this prop exists, so the same value is used for both.
  - **No regression for existing apps**: verified the generated XML is byte-for-byte identical to before this fix for any app that only configures the background color props and never sets `maneuverTurnIconColor` at all.
- **Fixed: the ETA bar could still render under the system navigation bar on some real devices even with the `WindowInsets` listener from 3.1.5 in place** — specifically observed on an Android 16 device, not reproduced on an Android 13 device with the same code. Root cause, confirmed against Expo SDK 53's own changelog: Android 16 disables the edge-to-edge opt-out attribute entirely (no app can avoid it, regardless of configuration), while Android 15 still allowed opting out and pre-15 devices never had this enforcement at all. On React Native's New Architecture with `react-native-safe-area-context` in the host app, the `WindowInsets` dispatch this package's listener depends on can be consumed higher up the RN view tree before ever reaching this package's nested native view — leaving `lastSystemBarInsets` stuck at `Insets.NONE` for the view's entire lifetime on affected devices.
  - **Fix**: added `fetchSystemBarInsetsDirectly()`, which reads system bar insets straight from the Activity's own `decorView` — found via `findActivity()`, which safely unwraps `ContextWrapper` layers (a plain `context as? Activity` cast is not reliable here, since the `Context` handed to an Expo/RN native view is frequently a `ReactContext`, not the `Activity` itself) — independently of whatever does or doesn't reach this view via RN's own dispatch chain. Called once during `buildUI()`, in addition to (not instead of) the existing listener, so devices where the listener already works correctly see no change at all.

### 3.1.5
**Two confirmed real-device-only issues resolved (both invisible on emulators, masking them until physical hardware testing), plus the maneuver color fix implemented.**

- **Root cause of the maneuver color investigation (2.3.9 through 3.1.4): confirmed to be system dark mode, not a code bug — and now fixed.** Directly confirmed: disabling system dark mode on the test device made `maneuverBackgroundColorDay` display correctly. Emulators commonly default to light mode, which is why this never reproduced there. Root cause: Mapbox ships its own `values-night/` qualified resources for `MapboxStyleManeuverView`, which Android selects automatically over this package's own PREVIOUSLY non-qualified (`values/` only) generated style whenever the system is in dark mode — regardless of what either of this package's two color mechanisms (`ManeuverViewOptions`, confirmed executing correctly in 3.1.3/3.1.4's diagnostic trace, and the `MapboxCustomManeuverStyle` XML style added in 3.1.4) set.
  - **Fix**: the config plugin now generates `mapbox_main_maneuver_background_color` and `MapboxCustomManeuverStyle` in **both** `res/values/` (day) and `res/values-night/` (night), using Android's own standard resource qualifier system — the same mechanism Mapbox's own SDK uses internally. Night resource priority mirrors the native `resolveManeuverBackgroundColor()`'s own fallback exactly: an explicit `androidColorOverrides` value (if set) applies to both variants; otherwise `maneuverBackgroundColorNight` is used; otherwise it falls back to `maneuverBackgroundColorDay`. Tested all three priority scenarios (both colors set, night omitted, explicit override) before shipping.
- **Fixed: the ETA bar and speed limit panel could render fully or partially underneath the system navigation bar (3-button or gesture) on real devices — confirmed working in an emulator, confirmed missing on physical hardware.** Neither element previously accounted for `WindowInsets` at all. Emulators' default navigation bar configuration often reserves little or no bottom screen space, masking this; real devices generally do reserve real space there, and Android 15+ makes edge-to-edge display mandatory rather than optional, making this fix necessary independent of any specific device. Added a `WindowInsets` listener (`ViewCompat.setOnApplyWindowInsetsListener`) that keeps the current system bar insets in sync and applies them as extra margin to both elements — re-evaluated automatically on rotation or navigation bar visibility changes, not just once at launch.
- **3D puck crash**: added success-path diagnostic logging (`.glb` header validated, file size, `LocationPuck3D` object constructed) to `build3DPuck()`, in addition to the existing error-path logging. Reported: still crashing, with no log at all appearing beforehand — consistent with a native/GPU-side rendering failure (most likely during the actual camera zoom/render pass) that bypasses Kotlin exception handling entirely, rather than anything at model-loading/configuration time. **Not yet fixed** — no safe, confirmed action identified without a native crash log (which Kotlin-level logging cannot produce for this class of failure); the next diagnostic step remains testing with a known-good reference `.glb` model to isolate whether this is asset-specific or independent of the specific file.

### 3.1.4
**Definitive progress on the maneuver color investigation, from a clean logcat trace (crash fixed, buffer cleared before testing).**

- **Confirmed via full diagnostic trace: the native code path for `maneuverBackgroundColorDay` is 100% correct.** The setter receives the right value, `parseColorSafe` correctly parses it to a valid color int, `ManeuverViewOptions.maneuverBackgroundColor` is correctly set with it, and `rebuildManeuverView()` successfully swaps in the freshly-configured view — every step logged and verified against a real device trace. This rules out a data-flow bug in this package's own code entirely — the confirmed-correct API call simply doesn't visibly change the banner's background in this SDK version, despite Mapbox's own documentation describing `ManeuverViewOptions.maneuverBackgroundColor` as controlling exactly that.
- **Added a second, independent mechanism, applied alongside the existing one (not instead of it):** Mapbox's docs separately describe an XML-style-attribute-based approach (`<style parent="MapboxStyleManeuverView"><item name="maneuverViewBackgroundColor">...</item></style>`) for the same visual property. The config plugin now generates this style (`MapboxCustomManeuverStyle`) whenever a maneuver background color is configured, referencing the same `mapbox_main_maneuver_background_color` resource already written for the `androidColorOverrides` sync added in 3.1.2 (one color source, not a second competing one). The native side applies it via `ContextThemeWrapper` — a universal, always-safe Android technique for applying a style to a programmatically-constructed view, chosen specifically to avoid guessing at an alternate `MapboxManeuverView` constructor signature that could fail to compile if wrong. A new diagnostic log (`"MapboxCustomManeuverStyle lookup returned resId=..."`) confirms whether this style is actually found and applied at runtime.
- Confirmed (directly reported) that the 3.1.3 night-mode regression fix resolved the issue — the banner displays correctly during night mode again.

### 3.1.3
**Critical crash fix, a confirmed regression reverted, and diagnostic logging added — from a third round of real device testing plus a captured (if initially confusing) logcat trace.**

- **Fixed a real crash in `onDetachedFromWindow`**, found from a logcat trace showing an `AndroidRuntime` error at this exact function. Every cleanup call there (`speechApi.cancel()`, `voiceInstructionsPlayer.shutdown()`, `maneuverApi.cancel()`, `routeLineApi.cancel()`, `routeLineView.cancel()`) targets a `lateinit var` only actually assigned inside `initAPIs()`/`setupNavigation()` — if the view gets detached before that setup fully completes (a hot-reload/fast-refresh remount in development, or Android tearing down the Activity abnormally after some other crash mid-setup), calling these on an uninitialized `lateinit var` throws `UninitializedPropertyAccessException`, crashing the app a *second* time during cleanup — potentially masking whatever the original problem was, and very plausibly explaining why color-prop diagnostic logs never appeared in testing (the app was crashing before ever reaching that code path in some sessions). Each call now guarded with Kotlin's standard `::property.isInitialized` check.
- **Reverted a confirmed regression, directly reported**: the maneuver banner failing to display *at all* during night mode, compared against 2.3.9's known-good behavior (before this package ever touched `android/`). Root cause: 3.0.0 had added a `rebuildManeuverView()` call inside `checkAndSwitchDayNight()`, to make `maneuverBackgroundColorNight` take effect automatically on the time-based day/night switch — a behavior that did not exist before 3.0.0. That rebuild-on-transition interaction broke the banner's visibility entirely during night mode. Removed outright — restoring the exact pre-3.0.0 behavior for this function (it only reloads the map style, never touches the maneuver view) — rather than risk another unconfirmed fix attempt. Explicitly setting `maneuverBackgroundColorNight` via its own prop setter is unaffected; only the automatic switch tied to the time-based check was removed. **Trade-off, made deliberately**: the banner no longer automatically re-colors when night mode kicks in — this is a known, accepted gap until the underlying color-application question below is fully resolved, prioritized over an actively broken banner.
- **Added targeted diagnostic logging** (`Log.d(TAG, "ManeuverColorDebug: ...")`) at every step of the color-prop chain — the setter, the rebuild function, and right before the color is handed to `ManeuverViewOptions.Builder()` — to definitively trace, via `adb logcat`, whether `maneuverBackgroundColorDay`/`Night` actually reach native code with the correct value, still the open question pending confirmed results now that the crash above no longer interrupts testing before reaching this code path.
- Confirmed via logcat that the speed limit panel's "not visible" report is a genuine Mapbox map-data limitation (`"Speed info unavailable for this location/route segment"` diagnostic, already added in 3.0.0, observed firing during real testing) — not a bug in this package. No further action planned here without new evidence.
- Investigated a suggested community fix referencing `NavigationView.api.retrieveNavigationMapboxMap()`/`styleUrlDay`/`styleUrlNight` — confirmed these are from Mapbox's old, pre-v3 Navigation SDK API surface (`NavigationLauncher`-era), not applicable to this package's `MapboxNavigationCore`/`MapboxNavigationUIKit`-based v3 integration. Not implemented.

### 3.1.2
**Fixes from a second round of real Android device testing.**

- **Fixed: `maneuverBackgroundColorDay` appeared to have no effect when `androidColorOverrides.mapbox_main_maneuver_background_color` was also set.** These are two independent mechanisms controlling the same visual element — confirmed `mapbox_main_maneuver_background_color` is a real, intentional resource-override key documented since this package's origin project, a build-time Android resource override, separate from `maneuverBackgroundColorDay`'s runtime `ManeuverViewOptions` call. Fixed priority (explicit `androidColorOverrides` wins if set; otherwise `maneuverBackgroundColorDay` is automatically synced into the same resource; otherwise Mapbox's default) — see the new "Priority" section under Mapbox Native Colors in the README.
  - Found and fixed a real bug in the fix itself while testing: `maneuverBackgroundColorDay` was never actually destructured from the plugin's own `options` parameter, causing a `ReferenceError` the moment this new sync logic tried to read it. This config plugin file had never previously needed to read this prop (it's normally only consumed as a native view prop, never by the build-time config plugin) — caught by the functional test written for this exact change before shipping it, not by a user report.
- **Added a `.glb` magic-number validation for `navigationPuck3DModelPath`**, after a confirmed real crash report (no crash log available). Checks the first 4 bytes of a local file match the required "glTF" signature (per the glTF 2.0 binary format spec) before ever handing it to Mapbox's 3D renderer — rejects obviously-invalid files outright. **Honest limitation**: if the actual crash is a native/GPU-side rendering failure (plausible — such failures often aren't raised as a catchable JVM exception, and may happen asynchronously on a different thread than the one that requested the puck change), this validation reduces but cannot fully guarantee elimination of the risk. A real crash log would allow a more targeted fix; recommended if this persists.
- **Fixed (likely): the ETA bar not appearing at all, unrelated to any color prop.** Traced this using new evidence — the maneuver banner was confirmed to display correctly with real-time updates during testing, which rules out "navigation never actually started" as a shared cause with the speed limit panel (a theory from the previous release). That evidence pointed specifically at `showEta` (added in 3.1.1): its native `Prop` declaration used a non-nullable Kotlin `Boolean`. If Expo Modules' behavior for an *omitted* optional boolean prop is to invoke the setter with Kotlin's own default (`false`) rather than not calling it at all, this would silently force the ETA bar hidden immediately after view creation for anyone not explicitly passing `showEta={true}` — independent of route/navigation state, matching the exact symptom. Changed to a nullable `Boolean?`, with `null` now explicitly treated as "not set, keep the default (`true`)" — removes the ambiguity regardless of Expo's exact behavior here, rather than relying on it.
- **Speed limit panel still not visible: no new bug found this round.** Re-traced `speedInfoApi`/`speedInfoView`'s initialization and rendering path with the new "navigation is confirmed active" evidence in hand — found no additional issue beyond what 3.0.0 already fixed (the elevation/z-order fix) and already-documented (the existing diagnostic log for Mapbox `maxspeed` data unavailability). If this persists, the most useful next step is checking `adb logcat` for `"Speed info unavailable for this location/route segment"` specifically, to distinguish a data-availability issue (outside this package's control) from something else neither of us has spotted yet.

### 3.1.1
**Fixes from real Android device testing: several color props reported as "not working" were actually being silently dropped by a hex-parsing bug, one prop was missing from the published TypeScript types entirely, and a stale compiled output would have shipped the old types regardless of the `src/index.tsx` fix.**

- **Fixed: `maneuverBackgroundColorDay`, `maneuverBackgroundColorNight`, `maneuverTurnIconColor` (and every other color prop) silently failing if the hex string didn't include a leading `#`.** Android's `Color.parseColor()` requires the `#` and throws `IllegalArgumentException` without it — every call site already wrapped this in `try/catch`, so a hex string like `"1E2433"` (missing `#`) failed silently and fell back to Mapbox's default color, with no visible error. Added a centralized `parseColorSafe()` helper that normalizes a missing `#` before parsing (also logs a clear error for genuinely invalid input, rather than failing silently). Applied to every color prop: maneuver colors, `navigationPuckColor`, ETA bar colors, icon button colors.
- **Fixed: several props added in 3.0.0/3.1.0 (`maneuverBackgroundColorNight`, `navigationPuckColor`, `navigationPuckImagePath`, `navigationPuck3DModelPath`, `speedLimitPosition`) were missing from `MapboxNavigationViewProps` in `src/index.tsx`.** The component itself spreads all props through to native unconditionally (`<NativeView {...props} />`), so this wasn't why they didn't visually work (that was the hex-parsing bug above) — but it meant no TypeScript autocomplete or type-checking for these props, a real DX regression. All now declared with full JSDoc.
- **Fixed: the compiled `build/index.d.ts` (what actually ships and is imported by consumers — not `src/index.tsx` directly) was stale relative to the `src/index.tsx` fix above.** This package's `prepublishOnly` script (`npm run build`) only runs automatically on `npm publish`, not on `npm pack` — so a locally-packed `.tgz` for testing would have still shipped the old, incomplete compiled types even after fixing the source. Rebuilt and verified `build/index.js` is byte-identical to before (confirms the `src/index.tsx` changes were purely type-level, no runtime behavior change) while `build/index.d.ts` now correctly includes every new property.
- **Added `showEta`** (boolean, default `true`) — explicit control over whether the ETA/duration/distance bar is shown, requested directly. Applied immediately if navigation is already active, in addition to the existing automatic show/hide behavior driven by route changes.
- Added a (previously missing) `tsconfig.json`, reconstructed to match the existing compiled output's style (CommonJS, `esModuleInterop`, declarations) — needed to actually run this package's own `build` script.
- **On the ETA bar and speed limit panel not being visible at all during testing**: traced through `showUI()`/`hideUI()` and the routes-changed observer that triggers them — this logic appears correctly wired to become visible once a route is successfully fetched and displayed. If the ETA bar specifically still doesn't appear with `showEta` explicitly set to `true`, that would point to navigation not actually having started successfully (worth checking `onRoutesReady`/`onRoutesFailed` events) rather than a visibility-flag bug. The speed limit panel's visibility depends on Mapbox actually having `maxspeed` data for the specific road being tested — a data availability question already documented in the [16 KB Page Size] era changelog entries, unrelated to `speedLimitPosition` specifically.

### 3.1.0
**iOS parity for Android's recent maneuver/puck customization props, implemented where a confirmed, official v3 public API exists — and explicitly NOT implemented where it doesn't, rather than guessing at internal class names that could fail to compile the entire iOS build.**

- **Added `maneuverBackgroundColorNight`, `navigationPuckColor`, `navigationPuckImagePath`, `navigationPuck3DModelPath` on iOS.**
  - Maneuver banner colors: implemented via a `StandardDayStyle`/`StandardNightStyle` subclass pair styling `InstructionsBannerView` through `UIAppearance` — confirmed directly from Mapbox's own official v3 "User interface" and "Maps for navigation" guides, which show this exact class and this exact `Style.apply()` override pattern (including a full `CustomNightStyle` code sample used as the direct template here). Colors are read once, when `presentNavigationViewController()` constructs a new `NavigationViewController` — `NavigationOptions.styles` is a construction-time configuration, so changing these while navigation is already active takes effect on the next route, not instantly. A real architectural difference from Android's live-rebuildable maneuver view, not an oversight — documented in the README.
  - Location puck: implemented via `NavigationMapView.puckType` (`Puck2DConfiguration`/`Puck3DConfiguration`), confirmed via Mapbox's own official v3 guide (`navigationMapView.puckType = .puck3D(.navigationDefault)`) and several official Maps SDK examples for the exact `Puck2DConfiguration(bearingImage:)`/`Puck3DConfiguration(model:)`/`Model(uri:orientation:)` signatures used. Same precedence as Android (3D model > custom image, never tinted > color tint > default). Unlike maneuver colors, this **is** live-updatable while navigation is active — `puckType` is a plain mutable property on an already-existing `NavigationMapView`, no reconstruction needed.
  - `navigationPuckColor` tints a standard SF Symbol (`location.north.circle.fill`) rather than Mapbox's own built-in puck asset — there's no confirmed public API for that specific internal asset's name on iOS, unlike Android where `com.mapbox.navigation.R.drawable.mapbox_navigation_puck_icon` is a real, documented identifier. Achieves the same intent (a colored puck) via a guaranteed-safe building block instead of guessing at an unconfirmed resource name.
  - All file/model loading (`navigationPuckImagePath`, `navigationPuck3DModelPath`) wrapped in existence checks and safe, non-throwing APIs (`UIImage(contentsOfFile:)`, `FileManager.fileExists`) — mirrors Android's crash-safety guarantees exactly: any failure falls through to the next priority level rather than crashing or leaving the puck without an icon.
- **Deliberately NOT implemented on iOS: `maneuverTurnIconColor`, `speedLimitPosition`, `etaBarBackgroundColor`, `etaTextColor`, `iconButtonColor`, `iconButtonMutedColor`.** Extensive research (official Mapbox v3 docs, API references, GitHub changelogs) found no confirmed v3 `MapboxNavigationUIKit` class name for the specific turn-icon sub-component, the ETA/button sub-elements, or a public repositioning API for `SpeedLimitView` — all v2-era `ManeuverView`/similar references found could not be confirmed to still exist unrenamed in the current v3 SDK. Since `UIAppearance`'s `.appearance(for:whenContainedInInstancesOf:)` requires a compile-time-concrete type, referencing an incorrect or renamed class would fail to compile the entire iOS module — not just leave one prop non-functional. These remain stored-only (no-op), same as before this release, rather than risk that. Implementing them would require building a custom top/bottom banner via `NavigationOptions(topBanner:bottomBanner:)` instead of relying on the drop-in `NavigationViewController` — a substantially larger change than styling, out of scope here.
- iOS-specific caveats for all of the above now documented directly in the README's Color Customization section, replacing an outdated generic note that no longer reflected what is/isn't implemented.

### 3.0.0
**Major version bump — first release with direct Android modifications, made on explicit request. `android/build.gradle` and both Android Kotlin source files were touched this release; every other prior release left `android/` completely untouched.**

- **Fixed: `maneuverBackgroundColorDay` and `maneuverTurnIconColor` never visibly applying.** `ManeuverViewOptions` is construction-time-only — it's set once when `MapboxManeuverView` is created in `buildUI()`, which runs in `init{}`, before Expo/RN has delivered any props to the view. Both props were therefore always still `null` at the one point they were read. A prior attempt at fixing `maneuverBackgroundColorDay` called `maneuverView?.setBackgroundColor(...)` from the setter instead — but that colors the plain Android View background of the outer container, a different visual layer from the SDK's own internal panel that `ManeuverViewOptions.maneuverBackgroundColor` actually controls, so nothing visibly changed. `maneuverTurnIconColor`'s setter only ever stored the value — a complete no-op with no live-view update attempt at all.
  - Fix: extracted maneuver view creation into `createManeuverView()`/`rebuildManeuverView()`. Color prop setters (and the day/night switch, see below) now swap in a freshly-configured `MapboxManeuverView` at the exact same position/layout params/visibility — the only way to actually apply new `ManeuverViewOptions`, since they can't be updated on a live view.
- **Added `maneuverBackgroundColorNight`.** The component already auto-switches day/night mode based on time of day (`checkAndSwitchDayNight()`, 6am–8pm = day), but this previously only affected the map style — the maneuver banner's color was never wired to it at all. Now hooked in: `rebuildManeuverView()` is called from `checkAndSwitchDayNight()` too, and `resolveManeuverBackgroundColor()` picks the correct color based on current `isNightMode`.
- **Added `navigationPuckColor`, `navigationPuckImagePath`, `navigationPuck3DModelPath`** — full control over the location puck (the icon showing position/heading on the map, distinct from `maneuverTurnIconColor`). Clear precedence, mutually exclusive (3D model > custom image > color tint > Mapbox default), with multiple layers of crash-safety:
  - A custom image is never tinted, even if a color prop is also set — avoids any risk of a tint operation on an arbitrary user-supplied image.
  - File loading (`BitmapFactory.decodeFile`) and 3D model path validation are both wrapped in try/catch and existence checks, falling through to the next priority level (never crashing, never leaving the puck without an icon) on any failure.
  - **Fixed a real race condition found during audit**: puck settings are applied inside `setupNavigation()`'s *asynchronous* `mapboxMap.loadStyle(...)` callback (style loading isn't instant) — but prop setters can fire before that callback runs, since Expo delivers props right after native view creation, independently of map style load timing. Calling `mapView.location.updateSettings{}` before `setLocationProvider()` has run on the same plugin instance would have raced ahead of it. Added a `navigationSetupComplete` guard flag; setter-triggered re-applies are skipped until the initial setup has actually completed, since that initial call already picks up whatever the current prop values are by the time it runs.
- **Added `speedLimitPosition`** (`"bottomLeft"` default/unchanged, `"bottomRight"`, `"topLeft"`, `"topRight"`). `"topRight"` uses a wider margin to avoid the mute/overview/recenter button column already occupying that corner — an approximation based on the buttons' known size, not guaranteed pixel-perfect on every screen size.
- **Fixed: speed limit panel could render behind the ETA bar.** The ETA bar has an explicit `elevation` (8dp) and is added to the layout after the speed limit panel — both independently favor the ETA bar drawing on top when their bounds overlap. The speed limit panel had no elevation of its own (defaulting to 0); if the ETA bar's actual rendered height (`WRAP_CONTENT`, varies with content) ever extended further than the fixed margin assumed, the panel could end up hidden behind it even with correct visibility/data. Gave it a higher elevation (12dp) — guarantees correct stacking regardless of any such overlap, with no change to position/margins in the non-overlapping case.
- **Full audit performed on every change from this session plus the rest of the Android module** — confirmed: every declared `Prop` has exactly one matching setter and vice versa (no orphans in either direction), brace/parenthesis balance holds across both Kotlin files, no duplicate function declarations, and (aside from the race condition above) `buildUI()`'s two other new-view-lifecycle mechanisms (`rebuildManeuverView`, `applySpeedLimitPosition`) don't share that timing risk — both operate on views created synchronously in `buildUI()`, which always completes before any prop setter could possibly run, unlike the puck's async-callback-nested setup code.
- Lane guidance confirmed to already render correctly, as part of `MapboxManeuverView.renderManeuvers()`'s own internal layout (a single Mapbox SDK component handling primary instruction, turn icon, and lane guidance together) — not something requiring separate positioning code, provided `bannerInstructions(true)`/`roundaboutExits(true)` are set on the route request (already the case).
- Speed limit panel not displaying: two pre-existing, already-documented data-availability fixes (`overview("full")`, `annotations("maxspeed,...")`) were confirmed already in place, sourced from direct Mapbox Support correspondence and Mapbox GitHub issue #4069. If the panel still doesn't appear after the elevation fix above, check `adb logcat` for `"Speed info unavailable for this location/route segment"` — an existing diagnostic log this component already emits when Mapbox's own map data has no `maxspeed` annotation for the specific road segment, a data limitation outside this package's control, not a code bug.

### 2.3.9
- **Android: second fix to the same injection bug — the first fix (matching `ext {`) still didn't work, because a real user-provided root `android/build.gradle` has NO literal `ext { }` block anywhere in it at all.** Modern Expo templates set `compileSdkVersion`/`minSdkVersion`/etc. via a custom Gradle plugin (`expo-root-project` / `com.facebook.react.rootproject`, applied via `apply plugin:`) that injects these values programmatically — not via a plain-text `ext { }` DSL block. Confirmed by directly inspecting a real, user-provided file. Neither of the two regexes tried (`rootProject\.ext\s*{`, then `\bext\s*{`) had anything to match. Replaced the whole approach: instead of finding-and-injecting into a block that may not exist, `withProjectBuildGradle` now **appends** a brand new `ext { }` block at the end of the file — the same robust, no-fragile-matching approach `addAndroidConfig`'s `dependencySubstitution` injection already uses successfully elsewhere in this same plugin. Tested against the user's actual real root `build.gradle` content this time (not a guessed/reconstructed structure) — confirmed `mapboxMapsVersion`/`mapboxNavVersion`/`mapboxUseNdk27` all correctly appear in the generated file.
- **Android: critical fix — `mapboxMapsVersion`/`mapboxNavigationVersion`/`androidUseNdk27` were never actually being written to the app's root `android/build.gradle`, despite the console log claiming success.** `withProjectBuildGradle` searched for the literal text `rootProject.ext {` — which does not exist in Expo/RN-generated root `build.gradle` files. They use a plain `ext { }` block nested inside `buildscript { }` (the same block that already holds `compileSdkVersion`/`minSdkVersion`/etc.). `.replace()` with a non-matching regex silently returns the string unchanged in JavaScript — no error — so the `console.log` (which ran unconditionally afterward) kept printing the correctly *calculated* values even though nothing was ever written to the file. This meant `android/build.gradle`'s `safeExtGet(...)` calls always fell back to their defaults (`mapboxUseNdk27=false` in particular) no matter what was configured in `app.json` — the actual root cause of `androidUseNdk27: true` appearing to have no effect on Google Play's 16 KB page-size check for `libnavigator-android.so`, even when the build log looked correct.
  - Fixed the regex to match `ext {` (word-bounded, avoiding accidental matches inside longer identifiers).
  - Tested against an actually-representative Expo-generated root `build.gradle` structure this time (`buildscript { ext { ... } }`) — the earlier, oversimplified test mock used earlier in 2.3.8's testing never exercised this code path realistically, which is exactly why this bug wasn't caught sooner.
  - Added a runtime warning (`— WARNING: no \`ext {\` block found in root android/build.gradle, values NOT injected!`) that now prints if the injection ever fails again for any reason (e.g. a future Expo template change) — this class of bug will no longer fail silently.
- **Android: fixed a hardcoded `COMMON_VER = '24.11.3'` inside `addAndroidConfig`** (the config plugin's own, previously-unaudited `dependencySubstitution` block for `com.mapbox.maps:*`/`com.mapbox.common:*` — separate from, and unconditional unlike, the `androidUseNdk27`-gated `com.mapbox.navigationcore:*` substitution added in 2.3.7's `android/build.gradle`). This block runs on every prebuild regardless of `androidUseNdk27`, and was already partially dynamic (`MAPS_VER` read `mapboxMapsVersion` correctly) — but `COMMON_VER` was static, and was already slightly wrong even for the default Maps version (`11.11.0` should pair with Common `24.11.0`, not the hardcoded `24.11.3`).
  - Added `calculateMapboxCommonVersion()`, using Mapbox's confirmed synchronized-versioning scheme (Maps `11.x.y` ↔ Common `24.x.y`, identical minor/patch — verified against Mapbox's own compatibility table, every entry checked).
  - Both `MAPS_VER` and `COMMON_VER` are now fully dynamic from `mapboxMapsVersion`, tested end-to-end (functional plugin simulation confirms `mapboxMapsVersion: "11.14.0"` produces `common-ndk27:24.14.0` in the generated `build.gradle`).
  - **This package now has two independent 16 KB mechanisms** — worth understanding clearly: this one (Maps/Common, always on) and 2.3.7's `androidUseNdk27` (Navigation Core, opt-in). They target different Mapbox artifact groups and don't conflict, but both need a correct, verified version pair to work — see [16 KB Page Size](#16-kb-page-size-android-15).
- **Android: `androidUseNdk27` now auto-floors the calculated Navigation version at `3.11.0`** when no explicit `mapboxNavigationVersion` is passed. Previously, enabling `androidUseNdk27: true` with only the default `mapboxMapsVersion` (`"11.11.0"`) would auto-calculate `mapboxNavigationVersion` as `"3.8.0"` via the Phase 1/Phase 2 formula (correct for the non-ndk27 case, but `-ndk27` artifacts don't exist below `3.11.0`) — a guaranteed `Could not resolve com.mapbox.navigationcore:...-ndk27:3.8.0` build failure. `calculateAndroidNavVersion()` now takes the `androidUseNdk27` flag and raises the derived version to `3.11.0` in that specific case. Passing `mapboxNavigationVersion` explicitly still always takes priority over this — the floor only applies to the auto-calculated fallback. Tested: `androidUseNdk27: false` behavior is byte-identical to before (no regression); `androidUseNdk27: true` with default `mapboxMapsVersion` now yields `"3.11.0"` instead of the doomed `"3.8.0"`; an explicit `mapboxNavigationVersion` (e.g. `"3.15.0"`) is still honored exactly, with no flooring applied.

### 2.3.7
- **Android: `mapboxMapsVersion`/`mapboxNavigationVersion` are now genuinely dynamic** — previously, `android/build.gradle` hardcoded `com.mapbox.navigationcore:*` at `3.8.1` and `com.mapbox.maps:android` at `11.11.0` regardless of what you passed in `app.json`; those app.json values were silently ignored for this purpose (confirmed by direct comparison against this package's very first shipped version — the hardcoding was never dynamic, in any prior version, contrary to what the iOS SPM-era `mapboxNavigationVersion` param may have implied by name).
  - `android/build.gradle` now reads these via `safeExtGet('mapboxNavVersion', '3.8.1')` / `safeExtGet('mapboxMapsVersion', '11.11.0')` — the same standard Expo/RN convention already used there for `compileSdkVersion`/`minSdkVersion`/etc.
  - The config plugin (`withProjectBuildGradle`) writes `ext.mapboxMapsVersion` / `ext.mapboxNavVersion` into your app's own root `android/build.gradle` at prebuild time.
  - **Explicit request, exact behavior**: if you pass **both** `mapboxMapsVersion` and `mapboxNavigationVersion`, both are used exactly as given — no recalculation. If you pass **only** `mapboxMapsVersion`, `mapboxNavigationVersion` is auto-derived via the Phase 1/Phase 2 formula (`navMinor = mapsMinor - 3` below mapsMinor 16, aligned at/above it) — reconstructed from this package's earlier iOS version-pairing research, since Mapbox's actual release pattern was confirmed to follow this. This is a best-effort approximation (Mapbox's patch releases don't always follow it exactly) — pass both explicitly once you've verified a working pair.
  - Both code paths tested via simulation: explicit-both, auto-calculate-from-maps-only, and the Phase 1→Phase 2 boundary (`mapsMinor` 11/15/16/20) before shipping.
  - This does **not** by itself add 16 KB page-size support — it only makes the underlying dependency versions configurable, so you can point at a version (e.g. Navigation `3.11.0`+) that has `-ndk27` artifacts available, if you choose to use them.
- **Android: added `androidUseNdk27` option — 16 KB page-size support now integrated into the package itself**, instead of requiring a `dependencySubstitution` block hand-written in your own app's config plugin. Confirmed from Mapbox's own changelog: *"Navigation SDK Core Framework 3.11.0-beta.1 - 04 July, 2025 — Added support for Android 16 KB page-size devices. To consume SDK compatible with NDK 27 you need to add `-ndk27` suffix to the artifact name."* This does not exist for this package's default-pinned `3.8.1`.
  - `android/build.gradle`'s 6 Mapbox Gradle dependencies (`navigationcore:android/ui-components/tripdata/ui-maps/voice` + `maps:android`) now read a `MAPBOX_NDK27_SUFFIX` variable (via `safeExtGet('mapboxUseNdk27', false)`), appended to each artifact name.
  - **Default is `false` — zero behavior change for existing installs.** Verified: with the flag off, the generated dependency strings are byte-identical to the previous hardcoded ones (`com.mapbox.navigationcore:android:3.8.1`, not `...-ndk27:3.8.1`).
  - Enable via `"androidUseNdk27": true`, and pass a verified `mapboxNavigationVersion` (`>= "3.11.0"`) alongside it — this package's own Phase 1/Phase 2 auto-calculation is not reliable enough on its own for this; check Mapbox's release notes for your exact target version's tested Maps/Navigation pairing first. An unsupported pairing fails loudly (`Could not resolve`), not silently.
  - Only `android/build.gradle` was touched — no other Android files modified (verified via `diff` against this package's original, unmodified extraction).

### 2.3.6
- **iOS: found what looks like the ACTUAL root cause of `compiling for iOS 14.0, but module 'MapboxMaps' has a minimum deployment target of iOS 15.1`, after two earlier attempts (podspec `s.platforms`/`pod_target_xcconfig`, then `withXcodeProject` forcing the app's own project deployment target) both failed to resolve it.** Verified by directly inspecting the real downloaded binaries (not a guess):
  ```
  $ head -3 MapboxNavigationCore.framework/Modules/.../arm64-apple-ios.private.swiftinterface
  // swift-interface-format-version: 1.0
  // swift-compiler-version: Apple Swift version 5.9.2 (swiftlang-5.9.2.2.56 clang-1500.1.0.2.5)
  // swift-module-flags: -target arm64-apple-ios14.0 -enable-objc-interop ... -module-name MapboxNavigationCore
  ```
  **`-target arm64-apple-ios14.0` is hard-baked into the first line of every `.swiftinterface`/`.private.swiftinterface` file Mapbox ships in their official precompiled binaries for Navigation SDK 3.8.2** (compiled by Mapbox themselves with Xcode 15.1 / Swift 5.9.2 — visible in the same file's own metadata). This is a permanent property of that specific binary release. It lives inside the vendored `.xcframework` itself — **not** in anything CocoaPods, a Podfile, a podspec, or the consuming app's `project.pbxproj` controls, which would explain why neither prior attempt had any effect: neither could reach this file. Even `mapbox-navigation-ios-build-artifacts`'s latest `Package.swift` still only declares `platforms: [.iOS(.v14)]`, so a version bump alone wasn't a safe assumption either.
  - **Fix**: `ios/fetch-xcframeworks.sh` now patches every `.swiftinterface` file's embedded target triple (`arm64-apple-ios14.0`, `arm64-apple-ios14.0-simulator`, `x86_64-apple-ios14.0-simulator` — all three architecture slices) from `14.0` to `15.1` immediately after copying the frameworks into `ios/Frameworks/`. This only modifies this package's own vendored copies — never Mapbox's original downloaded artifacts.
  - **Verification status: confirmed via a real run of the GitHub Actions workflow** on the actual macOS runner (not just a local simulation) — the log shows `patched 30 .swiftinterface file(s)`, matching exactly. Spot-checked the merged result directly: `MapboxNavigationCore`'s `.private.swiftinterface` now reads `-target arm64-apple-ios15.1`, all three architecture slices (`arm64`, `arm64-simulator`, `x86_64-simulator`) confirmed patched, and the framework's Mach-O binary itself is untouched/uncorrupted (`file` still reports a valid dynamically linked shared library).
  - **Also fixed in this release**: the `withXcodeProject` fix from the prior attempt hadn't actually made it into the GitHub repo before the workflow ran (a gap caught during this release's audit) — added now, so both fixes ship together.
  - The `withXcodeProject` fix from the previous attempt (forcing the app's own project deployment target) is kept in place as a reasonable safety net for unrelated reasons, even though it turned out not to be what this specific error needed.

### 2.3.5
- **iOS: reverted the 2.3.4 deduplication — restored the explicit `IPHONEOS_DEPLOYMENT_TARGET` in `pod_target_xcconfig`.** 2.3.4 removed it on the theory that `s.platforms` alone would be sufficient and act as a single source of truth. **That theory was wrong.** Confirmed with a real `pod install` log showing `Installing ExpoMapboxNavigation (2.3.4)` — i.e. the fixed version genuinely was installed — yet the build still failed with `compiling for iOS 14.0, but module 'MapboxMaps' has a minimum deployment target of iOS 15.1`. `s.platforms` governs CocoaPods' own dependency-compatibility checks; it does not reliably override the actual `IPHONEOS_DEPLOYMENT_TARGET` build setting used to compile this target in this project. Both declarations are needed. The maintainer reminder in `ios/fetch-xcframeworks.sh` now says to update both when bumping the vendored SDK version, instead of just one.

### 2.3.4
- **iOS: fixed `compiling for iOS 14.0, but module 'MapboxMaps' has a minimum deployment target of iOS 15.1`.** Confirmed directly from a real build error (not a guess this time): our podspec declared `s.platforms = { :ios => '14.0' }`, but `MapboxMaps` 11.11.0 (installed by `@rnmapbox/maps`, and imported internally by our vendored `MapboxNavigationCore`/`MapboxNavigationUIKit`) requires iOS 15.1 minimum. Raised `s.platforms` and `IPHONEOS_DEPLOYMENT_TARGET` from `14.0` to `15.1`. This also retroactively explains the `failed to build module 'MapboxNavigationCore'; this SDK is not supported by the compiler` error from 2.3.3 — it was very likely a downstream symptom of this same deployment-target mismatch, not an independent Swift toolchain-version issue as that changelog entry speculated; confirmed once `no such module 'MapboxCommon_Private'`/`'Turf'` (the 2.3.3 fix) stopped appearing and this became the sole remaining error. This doesn't tighten your app's overall minimum iOS version beyond what `@rnmapbox/maps` already requires transitively via `MapboxMaps` — it was already effectively floored at 15.1.
- **iOS: removed the duplicate `IPHONEOS_DEPLOYMENT_TARGET` declaration** from `pod_target_xcconfig` — it was set in two places (there and `s.platforms`), which is exactly the kind of two-places-to-update setup that let this value go stale in the first place. `s.platforms` is now the single source of truth. `ios/fetch-xcframeworks.sh` also now carries an explicit maintainer reminder to check `s.platforms` against the target MapboxMaps release's own minimum whenever the vendored SDK version is bumped. Full dynamic auto-detection isn't practical here — CocoaPods podspecs don't have a standard way to introspect another pod's declared minimum before the dependency graph is resolved, and this package's own podspec is static, published content, not something our config plugin can regenerate at the consuming app's build time.

### 2.3.3
- **iOS: fixed `no such module 'MapboxCommon_Private.MBXLog_Internal'` / `no such module 'Turf'`** at Swift compile time. Our vendored `MapboxNavigationCore`/`MapboxDirections` xcframeworks' private Swift interfaces internally import `MapboxCommon_Private` and `Turf`, but the podspec declared no explicit CocoaPods dependency on those pods — so CocoaPods never wired up the module/header search paths needed for our target to resolve them, even though `@rnmapbox/maps` already installs the same pods elsewhere in the project. Added `s.dependency` on `MapboxCommon`, `MapboxCoreMaps`, `MapboxMaps`, and `Turf` (no version pin — reuses whatever version `@rnmapbox/maps` already resolves, avoiding a second conflicting constraint).
- **iOS: restored the `useFrameworks: "static"` documentation** in the installation instructions, removed by mistake in 2.3.0. This is a real requirement of the vendored-xcframework architecture (same as the original `youssefhenna/expo-mapbox-navigation` this package is based on). Note: on its own, this setting does **not** fix the `no such module` errors above — a user confirmed they already had it enabled and still hit the error. The actual fix for that is the `s.dependency` addition above.
- **iOS: attempted fix for `failed to build module 'MapboxNavigationCore'; this SDK is not supported by the compiler`.** This is a real, standard Swift/Xcode toolchain-version check (confirmed via multiple Swift Forums / Apple Developer Forums threads, and `mapbox/mapbox-maps-ios#1363` reporting the identical error against an earlier Mapbox binary distribution) — not a side-effect of the `no such module` errors above, as an earlier draft of this changelog incorrectly assumed. Debug builds default to `ENABLE_TESTABILITY = YES`, which makes Xcode re-verify vendored frameworks against their `.private.swiftinterface` (textual, testable interface) instead of just linking the precompiled `.swiftmodule` binary — and that re-verification is what triggers the strict compiler-version check if the Swift compiler that built Mapbox's official xcframeworks differs at all from the one running the build. Added `s.user_target_xcconfig = { 'ENABLE_TESTABILITY' => 'NO' }` to skip that recheck. This is scoped to how the app target treats vendored/third-party framework imports; it doesn't affect `@testable import` of your own app code. **Not yet confirmed against a real build** — report back if this does or doesn't resolve it.

### 2.3.2
- **Android: fixed `<service>` incorrectly placed as a direct child of `<manifest>` instead of `<application>`** — caused `AAPT: error: unexpected element <service> found in <manifest>` at build time. This was a pre-existing bug (present since the plugin's original Android implementation, unrelated to the 2.3.x iOS architecture changes), only now surfaced by a build that actually exercised this code path. `<uses-permission>` entries are unaffected — those are correctly direct children of `<manifest>` per the Android manifest schema; only `<service>`, along with other app components, must be nested inside `<application>`.

### 2.3.1
- **iOS: fixed `vendored_frameworks` using an absolute path**, which CocoaPods rejects outright (`File Patterns: File patterns must be relative and cannot start with a slash`). `Dir.glob` still resolves against the absolute package directory (needed for the glob to actually find files on disk), but the resulting paths are now stripped back to relative before being assigned to `s.vendored_frameworks`. This is the only change in this release — 2.3.0's architecture (vendored prebuilt xcframeworks) is otherwise unchanged.

### 2.3.0
- **iOS: complete architecture rewrite — vendored prebuilt xcframeworks, no more live SPM.** The 2.2.x `post_install` Ruby-hook approach (injecting SPM package references into your project at `pod install` time) has been replaced entirely. It proved structurally unreliable: React Native's own CocoaPods SPM manager (`react-native/scripts/cocoapods/spm.rb`) unconditionally strips any manually-added SPM package reference during `post_install` unless it's declared through React Native's own `spm_dependency()` API — and that API is itself documented to cause duplicate-symbol errors on statically-linked Expo modules (see [facebook/react-native#47344](https://github.com/facebook/react-native/issues/47344)).
- **iOS SDK binaries now fetched from Mapbox's official `mapbox-navigation-ios-build-artifacts`** and vendored directly in the npm package (`ios/Frameworks/*.xcframework`, via `s.vendored_frameworks` in the podspec). No network access to `api.mapbox.com`, no SPM resolution, and no CocoaPods/SPM interop machinery is needed at `pod install` or `xcodebuild` time for any consumer anymore.
- **New: `.github/workflows/build-xcframeworks.yml` + `ios/fetch-xcframeworks.sh`** — a maintainer-only, manually-triggered GitHub Actions workflow that fetches the official prebuilt binaries for a given Navigation SDK version tag and commits them to the repo. Runs on GitHub's free macOS runners (unlimited for public repos); no local Mac needed to cut a release.
- **iOS: `useFrameworks: "static"` is still required** (same as this package's `youssefhenna/expo-mapbox-navigation` origin) — see 2.3.4 below; this changelog entry originally claimed it was no longer needed, which was incorrect.
- **`mapboxNavigationVersion` plugin option deprecated.** The iOS SDK version is now fixed per npm package release rather than runtime-configurable; the option is still accepted (no breaking change to existing `app.json` configs) but has no effect.
- **`downloadsToken` no longer used at app-build time on iOS.** Still required by the plugin's validation (kept for backward compatibility) but nothing downloads from `api.mapbox.com` during your build anymore — it's only used by the maintainer's one-time GitHub Actions fetch, authenticated via its own separate secret.
- **`mapboxMapsVersion` is now Android-only.** iOS no longer reads it (the iOS SDK version is fixed by the vendored binaries, not calculated at build time).
- **Plugin simplified by ~250 lines** — all Xcodeproj/Ruby SPM-injection code removed from `plugin/src/index.js`. Android-side logic (`dependencySubstitution`, NDK/ABI config, color overrides, permissions) is unchanged.

### 2.2.8
- **iOS version strategy redesigned** — dynamic `mapboxNavigationVersion` calculation from `mapboxMapsVersion` minor. Prevents `MapboxCommon` version conflicts with `@rnmapbox/maps`. Pattern confirmed from real Mapbox releases: Navigation `3.N.x` always compatible with Maps `11.N.x`.
- **iOS: `mapboxNavigationVersion` optional param** — escape hatch to pin exact Navigation version.
- **iOS: post_install hook strengthened** — fallback `include?` target search + debug log of all available targets if `ExpoMapboxNavigation` not found.
- **Android color props fixed** — setters now apply immediately to views (were only stored previously, causing icon colors to remain default).

### 2.2.0
- **iOS support added** — full native implementation using `NavigationViewController` drop-in UI.
- iOS SPM integration via `post_install` Podfile hook (Xcodeproj Ruby API, same technique as `@rnmapbox/maps`).
- `downloadsToken` required for iOS SPM authentication.
- Fixed previous phantom `.xcframework` references that caused `Unimplemented component` crashes.

### 2.1.x
- Waze-style Android UI: maneuver banner, speed limit, ETA bar, action buttons (mute/overview/recenter).
- Voice instructions with TTS fallback.
- Puck jitter fix (GitHub issue #4140) — `keyPoints = emptyList()`.
- Lane guidance fix — explicit `bannerInstructions(true)`, `steps(true)`, `roundaboutExits(true)`.
- `onManeuverBannerPressed` event with full route steps list.
- Color customization props (Android).

### 2.0.1
- Fix #43: `CameraAnimationsUtils.calculateCameraAnimationHint` crash on Android.
- Fix #31: `voiceUnits` prop for metric/imperial.
- NDK 27 + 16 KB page size enforcement.
- Expo SDK 53 compatibility.

---

## License

MIT