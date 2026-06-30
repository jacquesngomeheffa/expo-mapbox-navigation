# @jacques_gordon/expo-mapbox-navigation

[![npm version](https://badge.fury.io/js/@jacques_gordon%2Fexpo-mapbox-navigation.svg)](https://www.npmjs.com/package/@jacques_gordon/expo-mapbox-navigation)

Expo module for Mapbox Navigation SDK — forked from [`@badatgil/expo-mapbox-navigation`](https://github.com/uju777/expo-mapbox-navigation) with the following fixes and improvements:

| Change | Details |
|--------|---------|
| 🐛 **Fix #43** | Android crash: `NoSuchMethodError` for `CameraAnimationsUtils.calculateCameraAnimationHint` — caused by Mapbox Maps/Navigation SDK version mismatch. Fixed by pinning `mapbox-maps-android ≥ 11.11.0` and Navigation SDK `3.7.0`. |
| 🐛 **Fix #31** | Voice instructions always defaulted to imperial units. New `voiceUnits` prop (`"metric"` \| `"imperial"`) added. |
| ✅ **NDK 27** | Forced NDK `27.0.12077973` for full 16 KB page size compatibility (Android 15+ requirement). |
| ✅ **16 KB page size** | `jniLibs.useLegacyPackaging = false` + `ANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON` — required for Google Play compliance from 2025 onwards. |
| ✅ **Expo SDK 53+** | Compatible with Expo SDK ≥ 53 and React Native 0.79+. |
| ✅ **Maps v11.11.0** | Minimum Mapbox Maps Android SDK enforced to 11.11.0 (config plugin validates this). |
| ✅ **iOS support** | Full native iOS implementation (`NavigationViewController` drop-in UI) via Swift Package Manager — feature parity with Android (lane guidance, speed limit, voice instructions, day/night, steps list). |

---

## Installation

```bash
npx expo install @jacques_gordon/expo-mapbox-navigation @rnmapbox/maps
```

### Setup @rnmapbox/maps first

Follow the [full @rnmapbox/maps installation guide](https://rnmapbox.github.io/docs/install). Set `RNMapboxMapsVersion` to `11.11.0` or higher.

```json
"plugins": [
  [
    "@rnmapbox/maps",
    {
      "RNMapboxMapsImpl": "mapbox",
      "RNMapboxMapsVersion": "11.11.0",
      "RNMapboxMapsDownloadToken": "sk.your_secret_token"
    }
  ]
]
```

### Add this plugin

```json
"plugins": [
  [
    "@jacques_gordon/expo-mapbox-navigation",
    {
      "accessToken": "pk.your_public_token",
      "downloadsToken": "sk.your_secret_token",
      "mapboxMapsVersion": "11.11.0"
    }
  ]
]
```

> ⚠️ `mapboxMapsVersion` must match the version set in `@rnmapbox/maps`. Minimum: `11.11.0`.

> ⚠️ `downloadsToken` is **required** — it's a secret Mapbox token (starts with `sk.`) with the **Downloads:Read** scope. On iOS it's used to authenticate Swift Package Manager when it fetches the Mapbox Navigation SDK from Mapbox's private package registry (via a generated `.netrc` entry). This is the same token already used for `RNMapboxMapsDownloadToken` above — you can reuse it.

### iOS architecture

iOS uses the **official Mapbox Navigation SDK v3 drop-in UI** (`NavigationViewController`), installed via **Swift Package Manager** — not prebuilt `.xcframework` binaries. This is the only officially supported distribution method for the Navigation SDK v3 on iOS; CocoaPods doesn't host it directly, so this package bridges it in automatically through CocoaPods' `spm_dependency()` mechanism when you run `pod install` / `expo prebuild`.

Because `NavigationViewController` is a complete drop-in experience, lane guidance, speed limit display, voice instructions, and the recenter/overview camera button all come built-in from Mapbox — no extra wiring needed, unlike the more manual Android implementation.

### iOS: enable static frameworks

```json
"plugins": [
  ["expo-build-properties", { "ios": { "useFrameworks": "static" } }]
]
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
        { latitude: 48.8566, longitude: 2.3522 },  // Paris
        { latitude: 51.5074, longitude: -0.1278 },  // London
      ]}
      voiceUnits="metric"           // Fix for issue #31
      language="fr"
      navigationProfile="driving-traffic"
      onArrival={() => console.log('Arrived!')}
      onRoutesFailed={({ nativeEvent }) =>
        console.error('Routes failed:', nativeEvent.message)
      }
    />
  );
}
```

---

## Props

| Prop | Type | Default | Description |
|------|------|---------|-------------|
| `coordinates` | `Coordinate[]` | required | Route waypoints. Min 2 items. |
| `waypointIndices` | `number[]` | all points | Which coordinates are waypoints. |
| `language` | `string` | device locale | BCP-47 locale (e.g. `"fr"`, `"en-US"`). |
| `voiceUnits` | `"metric" \| "imperial"` | auto | **Fix #31** — Voice/distance units. |
| `navigationProfile` | `string` | `"driving-traffic"` | Mapbox routing profile. |
| `excludeTypes` | `string[]` | — | Road types to avoid. |
| `mapStyle` | `string` | Mapbox Navigation Day | Map style URL. |
| `mute` | `boolean` | `false` | Silence voice instructions. |
| `maxHeight` | `number` | — | Max vehicle height (m). |
| `maxWidth` | `number` | — | Max vehicle width (m). |
| `useMapMatching` | `boolean` | `false` | Use Map Matching API. |
| `customRasterTileUrl` | `string` | — | Custom tile URL with `{x}/{y}/{z}`. |
| `customRasterAboveLayerId` | `string` | — | Layer ID to place custom raster above. |

---

## Events

| Event | Payload | Description |
|-------|---------|-------------|
| `onRoutesReady` | `{ routeCount, distanceMeters, durationSeconds }` | Routes calculated. |
| `onRouteProgressChanged` | `{ distanceRemaining, durationRemaining, ... }` | Progress update. |
| `onArrival` | `{}` | User reached destination. |
| `onNavigationCancelled` | `{}` | User cancelled navigation. |
| `onNavigationFinished` | `{}` | Session ended normally. |
| `onRoutesFailed` | `{ message }` | Route calculation failed. |

---

## Android Color Overrides

Customize the Navigation UI colors by overriding Mapbox resource values:

```json
["@jacques_gordon/expo-mapbox-navigation", {
  "accessToken": "pk.your_token",
  "mapboxMapsVersion": "11.11.0",
  "androidColorOverrides": {
    "mapbox_main_maneuver_background_color": "#FF5500",
    "mapbox_primary_route_color": "#0055FF"
  }
}]
```

---

## 16 KB Page Size Compatibility

Android 15 (API 35) requires all `.so` native libraries to be aligned to 16 KB boundaries for devices using 16 KB memory page sizes.

This package enforces:
- **NDK 27** (`27.0.12077973`) — the first NDK version with full 16 KB support
- **`jniLibs.useLegacyPackaging = false`** — prevents `.so` compression, enabling proper alignment
- **64-bit-only ABI filters** (`arm64-v8a`, `x86_64`) — 16 KB requirement applies to 64-bit only

More info: [Android 16 KB page size guide](https://developer.android.com/guide/practices/page-sizes)

---

## Changelog

### 2.2.0
- **iOS support added.** Full native implementation using Mapbox Navigation SDK v3 (`NavigationViewController` drop-in UI) via Swift Package Manager.
- Fixed: previous versions referenced non-existent `.xcframework` files in the podspec, causing `Unimplemented component: ViewManagerAdapter_ExpoMapboxNavigation` crashes on iOS — no native module was ever actually registered.
- New required `downloadsToken` config plugin option (secret Mapbox token, used to authenticate Swift Package Manager).
- iOS feature parity with Android: lane guidance, speed limit, voice instructions (with working mute), day/night auto-switching, and the `onManeuverBannerPressed` full-steps-list event.

### 2.0.1
- Fix #43: `CameraAnimationsUtils.calculateCameraAnimationHint` NoSuchMethodError on Android
- Fix #31: Add `voiceUnits` prop for metric/imperial voice instructions
- Force NDK 27 for 16 KB page size support
- Enforce Mapbox Maps Android ≥ 11.11.0
- Expo SDK 53 compatibility

---

## License

MIT
