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

That's it — as of **2.3.0**, no `useFrameworks`/static-linkage configuration is required by this package specifically (see [iOS Architecture](#ios-architecture) below for why). If another dependency in your project needs `expo-build-properties`'s `useFrameworks`, that's unrelated to this package and can be configured independently.

---

## Plugin Options

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `accessToken` | ✅ | — | Public Mapbox token (`pk.*`). Used for map tiles and routing. |
| `downloadsToken` | ✅ | — | Secret Mapbox token (`sk.*`) with **Downloads:Read** scope. Same token as `RNMapboxMapsDownloadToken`. Kept for backward compatibility with app.json configs from earlier versions — as of 2.3.0 it's no longer used at app-build time (the iOS SDK is vendored as prebuilt binaries; nothing downloads from `api.mapbox.com` during your `pod install`/EAS build anymore). |
| `mapboxMapsVersion` | ✅ | `"11.11.0"` | Must exactly match `RNMapboxMapsVersion` in `@rnmapbox/maps`. **Android only** as of 2.3.0 (used to pick compatible native resource versions). iOS SDK version is fixed per npm package release — see [iOS Architecture](#ios-architecture). |
| `mapboxNavigationVersion` | — | — | **Deprecated, no longer used.** Accepted for backward compatibility only; safe to remove from your config. iOS Navigation SDK version is now fixed by which npm package version you install, not runtime-configurable. |
| `androidColorOverrides` | — | `{}` | Override Mapbox native resource colors on Android. |

---

## iOS Architecture

### How it works (as of 2.3.0)

Mapbox Navigation SDK v3 for iOS is distributed via Swift Package Manager only — Mapbox has not shipped CocoaPods support for it. Earlier versions of this package (2.2.x) tried to bridge SPM into CocoaPods live, at your app's `pod install` time, using the same `post_install` Ruby-hook technique `@rnmapbox/maps` uses for its own dependencies. That approach turned out to be fundamentally unreliable in practice: React Native's own SPM manager silently strips manually-added SPM package references during `pod install`, and the officially-sanctioned alternative (`spm_dependency()`) is documented to cause duplicate-symbol errors on statically-linked Expo modules.

**2.3.0 takes a different approach: the iOS SDK binaries are prebuilt and vendored directly into this npm package.** Mapbox officially publishes `MapboxNavigationCore`/`MapboxNavigationUIKit`/`MapboxDirections` (and their transitive binary dependencies) as precompiled `.xcframework` downloads via a dedicated repository, [`mapbox-navigation-ios-build-artifacts`](https://github.com/mapbox/mapbox-navigation-ios-build-artifacts). This package's maintainer fetches those once per Navigation SDK version (via [`.github/workflows/build-xcframeworks.yml`](.github/workflows/build-xcframeworks.yml) on a free GitHub-hosted macOS runner) and commits them into `ios/Frameworks/`, which the podspec vendors via `s.vendored_frameworks`.

**What this means for you:**
- No network access to `api.mapbox.com` needed during your `pod install` or EAS build.
- No SPM package resolution happens in your project for this SDK at all.
- No `useFrameworks`/static-linkage configuration is required by this package.
- The iOS SDK version is fixed by which version of this npm package you install (matching a specific `mapboxMapsVersion`), not something you configure per-app.

**Why `MapboxMaps`/`MapboxCommon`/`MapboxCoreMaps`/`Turf` are *not* vendored here:** `@rnmapbox/maps` already installs those via CocoaPods. Vendoring a second copy of the same libraries would cause duplicate-symbol link errors. Only the Navigation-specific frameworks that `@rnmapbox/maps` doesn't already provide are vendored by this package.

### Upgrading the vendored iOS SDK version (maintainers)

The iOS binaries are tied to a specific Navigation SDK version, matched to a specific `MapboxMaps` version (see `MAPBOX_NAV_VERSION`/`MAPBOX_MAPS_VERSION`/`MAPBOX_COMMON_VERSION` in [`ios/fetch-xcframeworks.sh`](ios/fetch-xcframeworks.sh)). To bump:

1. Confirm the target Navigation version's matching Maps/Common versions (check `mapbox-navigation-ios-build-artifacts` release notes, or the `pod install` log of a project using the target `RNMapboxMapsVersion`).
2. Update the version constants at the top of `ios/fetch-xcframeworks.sh`.
3. Run the **"Build Mapbox Navigation xcframeworks"** GitHub Actions workflow with the new version tag.
4. Merge the resulting `xcframeworks/<version>` branch.
5. `npm version` + `npm publish` as usual.

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
| `maneuverBackgroundColorDay` | Mapbox default | Background of the turn-by-turn instruction banner. Uses `ManeuverViewOptions.maneuverBackgroundColor` (official Mapbox SDK API). |
| `maneuverTurnIconColor` | Mapbox default | Color of the turn arrow icon. Uses `ManeuverViewOptions.turnIconManeuver`. |
| `etaBarBackgroundColor` | `"#1E2433"` | Background of the bottom ETA/duration/distance bar. |
| `etaTextColor` | `"#FFFFFF"` | Text color for ETA time and duration. |
| `iconButtonColor` | `"#1A73E8"` | Color of the mute/overview/recenter buttons (default state). |
| `iconButtonMutedColor` | `"#EA4335"` | Color of the mute button when voice is muted. |

```tsx
<MapboxNavigationView
  maneuverBackgroundColorDay="#1E2433"
  maneuverTurnIconColor="#1A73E8"
  etaBarBackgroundColor="#1E2433"
  etaTextColor="#FFFFFF"
  iconButtonColor="#1A73E8"
  iconButtonMutedColor="#EA4335"
/>
```

> **Note:** On iOS, `NavigationViewController` applies its own theme. Color props are stored and can be applied via the SDK's `StyleManager` in a future release.

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

This package enforces full compliance with Android's 16 KB memory page size requirement:

- **NDK 27** (`27.0.12077973`) — first NDK version with full 16 KB support
- **`jniLibs.useLegacyPackaging = false`** — prevents `.so` compression, enables proper alignment
- **64-bit ABI filters** (`arm64-v8a`, `x86_64`) — requirement applies to 64-bit only
- **NDK27 variant substitution** — `dependencySubstitution` replaces all Mapbox Maven artifacts with their `-ndk27` equivalents across the entire dependency graph (including transitive deps from other packages)

See [Android 16 KB page size guide](https://developer.android.com/guide/practices/page-sizes).

---

## Changelog

### 2.3.1
- **iOS: fixed `vendored_frameworks` using an absolute path**, which CocoaPods rejects outright (`File Patterns: File patterns must be relative and cannot start with a slash`). `Dir.glob` still resolves against the absolute package directory (needed for the glob to actually find files on disk), but the resulting paths are now stripped back to relative before being assigned to `s.vendored_frameworks`. This is the only change in this release — 2.3.0's architecture (vendored prebuilt xcframeworks) is otherwise unchanged.

### 2.3.0
- **iOS: complete architecture rewrite — vendored prebuilt xcframeworks, no more live SPM.** The 2.2.x `post_install` Ruby-hook approach (injecting SPM package references into your project at `pod install` time) has been replaced entirely. It proved structurally unreliable: React Native's own CocoaPods SPM manager (`react-native/scripts/cocoapods/spm.rb`) unconditionally strips any manually-added SPM package reference during `post_install` unless it's declared through React Native's own `spm_dependency()` API — and that API is itself documented to cause duplicate-symbol errors on statically-linked Expo modules (see [facebook/react-native#47344](https://github.com/facebook/react-native/issues/47344)).
- **iOS SDK binaries now fetched from Mapbox's official `mapbox-navigation-ios-build-artifacts`** and vendored directly in the npm package (`ios/Frameworks/*.xcframework`, via `s.vendored_frameworks` in the podspec). No network access to `api.mapbox.com`, no SPM resolution, and no CocoaPods/SPM interop machinery is needed at `pod install` or `xcodebuild` time for any consumer anymore.
- **New: `.github/workflows/build-xcframeworks.yml` + `ios/fetch-xcframeworks.sh`** — a maintainer-only, manually-triggered GitHub Actions workflow that fetches the official prebuilt binaries for a given Navigation SDK version tag and commits them to the repo. Runs on GitHub's free macOS runners (unlimited for public repos); no local Mac needed to cut a release.
- **iOS: `useFrameworks`/static-linkage configuration no longer required.** Removed from installation instructions — this package's own linkage no longer depends on your project's SPM/CocoaPods interop settings.
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
