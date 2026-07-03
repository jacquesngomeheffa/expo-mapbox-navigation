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
| `mapboxMapsVersion` | ✅ | `"11.11.0"` | Must exactly match `RNMapboxMapsVersion` in `@rnmapbox/maps`. **Android only** as of 2.3.0. As of 2.3.7, this genuinely drives the `com.mapbox.maps:android` and (indirectly, via `mapboxNavigationVersion`'s auto-calculation) `com.mapbox.navigationcore:*` Gradle dependency versions — previously this option was silently ignored for that purpose and those versions were hardcoded. iOS SDK version is fixed per npm package release — see [iOS Architecture](#ios-architecture). |
| `mapboxNavigationVersion` | — | auto-calculated (Android only) | **Android only** (reactivated in 2.3.7 — was deprecated/unused after 2.3.0's iOS rewrite). If set, used exactly as given for `com.mapbox.navigationcore:*` Gradle dependencies — no recalculation. If omitted, derived from `mapboxMapsVersion` via the Phase 1/Phase 2 formula (see [iOS Architecture](#ios-architecture) history) — a best-effort approximation, not a guarantee. Has no effect on iOS; the iOS SDK version is fixed by which npm package version you install. |
| `androidColorOverrides` | — | `{}` | Override Mapbox native resource colors on Android. |

---

## iOS Architecture

### How it works (as of 2.3.0)

Mapbox Navigation SDK v3 for iOS is distributed via Swift Package Manager only — Mapbox has not shipped CocoaPods support for it. Earlier versions of this package (2.2.x) tried to bridge SPM into CocoaPods live, at your app's `pod install` time, using the same `post_install` Ruby-hook technique `@rnmapbox/maps` uses for its own dependencies. That approach turned out to be fundamentally unreliable in practice: React Native's own SPM manager silently strips manually-added SPM package references during `pod install`, and the officially-sanctioned alternative (`spm_dependency()`) is documented to cause duplicate-symbol errors on statically-linked Expo modules.

**2.3.0 takes a different approach: the iOS SDK binaries are prebuilt and vendored directly into this npm package.** Mapbox officially publishes `MapboxNavigationCore`/`MapboxNavigationUIKit`/`MapboxDirections` (and their transitive binary dependencies) as precompiled `.xcframework` downloads via a dedicated repository, [`mapbox-navigation-ios-build-artifacts`](https://github.com/mapbox/mapbox-navigation-ios-build-artifacts). This package's maintainer fetches those once per Navigation SDK version (via [`.github/workflows/build-xcframeworks.yml`](.github/workflows/build-xcframeworks.yml) on a free GitHub-hosted macOS runner) and commits them into `ios/Frameworks/`, which the podspec vendors via `s.vendored_frameworks`.

**What this means for you:**
- No network access to `api.mapbox.com` needed during your `pod install` or EAS build.
- No SPM package resolution happens in your project for this SDK at all.
- `useFrameworks: "static"` is still required (same as the original `youssefhenna/expo-mapbox-navigation` this package builds on) — see step 3 of [Installation](#installation).
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

### 2.3.8
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
