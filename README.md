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

---

## Plugin Options

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `accessToken` | ✅ | — | Public Mapbox token (`pk.*`). Used for map tiles and routing. |
| `downloadsToken` | ✅ | — | Secret Mapbox token (`sk.*`) with **Downloads:Read** scope. Same token as `RNMapboxMapsDownloadToken`. Used on iOS to authenticate SPM when fetching the Navigation SDK from `api.mapbox.com` via `~/.netrc`. |
| `mapboxMapsVersion` | ✅ | `"11.11.0"` | Must exactly match `RNMapboxMapsVersion` in `@rnmapbox/maps`. |
| `mapboxNavigationVersion` | — | auto-calculated | iOS only. See [iOS Version Strategy](#ios-version-strategy) below. |
| `androidColorOverrides` | — | `{}` | Override Mapbox native resource colors on Android. |

---

## iOS Architecture

### How it works

iOS uses `NavigationViewController` — the official Mapbox Navigation SDK v3 drop-in UI — installed via **Swift Package Manager** (SPM). CocoaPods does not host the Navigation SDK v3 (Mapbox confirmed CocoaPods support is "coming soon").

This package bridges SPM into your CocoaPods/Expo project using a **`post_install` Ruby hook** injected into your `Podfile` — the same technique used by `@rnmapbox/maps` itself. The hook uses the Xcodeproj Ruby API (`XCRemoteSwiftPackageReference`, `XCSwiftPackageProductDependency`) to add the package properly, with find-or-create semantics to prevent duplicate symbols.

### iOS Version Strategy

This is the most important part. Understanding it prevents build failures.

**The problem:** SPM requires all packages in the dependency graph to agree on a single version of shared libraries (`MapboxCommon`, `MapboxMaps`, `Turf`). Both `@rnmapbox/maps` and `mapbox-navigation-ios` depend on these shared libraries. If they request incompatible versions, SPM fails.

**The Mapbox versioning pattern** (confirmed from official GitHub releases):

The `.0` release of each Navigation minor always pairs with the matching Maps minor:

| Maps version | Navigation `.0` | Compatible? |
|---|---|---|
| `11.11.0` | `3.11.0` | ✅ |
| `11.12.0` | `3.12.0` | ✅ |
| `11.21.5` | `3.21.5` (same minor+patch) | ✅ |

**⚠️ Patch versions drift.** Navigation patch releases (`3.11.x` where x > 0) often update to a newer Maps version — for example `3.11.4` requires Maps `11.14.7`, not `11.11.x`. Using `upToNextMinorVersion` would therefore be unsafe.

**Our solution:** We use the **exact** `.0` version that matches your Maps minor:

```
mapboxMapsVersion = "11.11.0"
  → Navigation exact version = "3.11.0"
  → requires MapboxMaps 11.11.x  ✅ compatible
```

This is **fully automatic** — when you upgrade Maps from `11.11.0` to `11.12.0`, the Navigation version is recalculated to exact `3.12.0`.

**Manual override (escape hatch):** To pin any specific Navigation version:

```json
["@jacques_gordon/expo-mapbox-navigation", {
  "accessToken": "pk.xxx",
  "downloadsToken": "sk.xxx",
  "mapboxMapsVersion": "11.11.0",
  "mapboxNavigationVersion": "3.11.2"
}]
```

During `expo prebuild`, you will see in the logs:
```
[@jacques_gordon/expo-mapbox-navigation] Maps 11.11.0 → auto-calculated Navigation 3.11.0..<3.12.0
[@jacques_gordon/expo-mapbox-navigation] ✅ Wrote Mapbox credentials to ~/.netrc
[@jacques_gordon/expo-mapbox-navigation] ✅ Injected mapbox-navigation-ios SPM hook into Podfile
```

And during `pod install`:
```
[ExpoMapboxNavigation] Added mapbox-navigation-ios to pods_project
[ExpoMapboxNavigation] Found target: ExpoMapboxNavigation
[ExpoMapboxNavigation] Linked MapboxNavigationCore -> ExpoMapboxNavigation
[ExpoMapboxNavigation] Linked MapboxNavigationUIKit -> ExpoMapboxNavigation
[ExpoMapboxNavigation] Linked MapboxNavigationCore -> Navio
[ExpoMapboxNavigation] Linked MapboxNavigationUIKit -> Navio
```

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
