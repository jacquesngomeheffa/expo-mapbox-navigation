import {
  requireNativeViewManager,
  NativeModulesProxy,
  EventEmitter,
} from 'expo-modules-core';
import React from 'react';
import { View, ViewStyle, StyleSheet } from 'react-native';

// ─────────────────────────────────────────────────────────────────────────────
// Types
// ─────────────────────────────────────────────────────────────────────────────

export interface Coordinate {
  latitude: number;
  longitude: number;
}

/**
 * Voice/distance unit system for navigation instructions.
 *
 * Fixes Issue #31: previously always defaulted to imperial.
 * - "metric"   → km/m for distances, km/h for speed
 * - "imperial" → mi/ft for distances, mph for speed
 * - undefined  → auto-detected from device locale
 */
export type VoiceUnits = 'metric' | 'imperial';

export type NavigationProfile =
  | 'driving-traffic'
  | 'driving'
  | 'walking'
  | 'cycling';

export interface MapboxNavigationViewProps {
  /** Navigation route waypoints. Minimum 2 coordinates required. */
  coordinates: Coordinate[];

  /**
   * Indices into `coordinates` that are considered waypoints.
   * First and last must be included. Defaults to all points.
   */
  waypointIndices?: number[];

  /**
   * BCP-47 language/locale tag for map labels and voice instructions.
   * Example: "fr", "en-US", "de-DE". Defaults to device locale.
   */
  language?: string;

  /**
   * Unit system for voice instructions and distance display.
   * - "metric"   → kilometres and metres (default outside US/UK)
   * - "imperial" → miles and feet (default in US/UK)
   * - undefined  → auto-detected from device locale
   *
   * Fixes Issue #31.
   */
  voiceUnits?: VoiceUnits;

  /**
   * Mapbox Directions profile.
   * Android: omit the "mapbox/" prefix (handled internally).
   */
  navigationProfile?: NavigationProfile;

  /** Road / feature types to exclude from the route */
  excludeTypes?: string[];

  /** Mapbox map style URL. Defaults to Mapbox Navigation Day style. */
  mapStyle?: string;

  /** Whether audio is initially muted. Defaults to false. */
  mute?: boolean;

  /** Maximum vehicle height in metres (for height-restricted routes). */
  maxHeight?: number;

  /** Maximum vehicle width in metres (for width-restricted routes). */
  maxWidth?: number;

  /**
   * Padding (in dp on Android, points on iOS) around the camera's framed
   * viewport, applied on top of the safe area. The navigation icon's
   * vertical position is primarily controlled by `Bottom` — a larger
   * value pushes it further up from the screen's true bottom edge.
   * Defaults to this package's existing values on each platform (unset by
   * default = no change from previous versions):
   * Android — top 180, left 40, bottom 300, right 40 (the Waze-style
   * ~30%-from-bottom position already used since 5.0.0).
   * iOS — top 20, left 20, bottom 40, right 20 (the Mapbox SDK's own
   * default; this package never overrode it before 5.1.0).
   * Applied live; does not trigger a route recalculation.
   */
  navigationViewportPaddingTop?: number;
  navigationViewportPaddingLeft?: number;
  navigationViewportPaddingBottom?: number;
  navigationViewportPaddingRight?: number;

  /**
   * When true, uses the Mapbox Map Matching API instead of the standard
   * Directions API — for routes that exactly follow the given coordinates
   * (implemented on both platforms as of 5.0.4; changing it re-triggers
   * the route request). Note: `excludeTypes`, `maxHeight` and `maxWidth`
   * are Directions-API-only request parameters and have no Map Matching
   * equivalent — they are ignored in this mode.
   */
  useMapMatching?: boolean;

  /**
   * URL template for a custom raster tile overlay (implemented on both
   * platforms as of 5.0.4). Must include {x}, {y}, {z} placeholders.
   * Example: "https://tile.openstreetmap.org/{z}/{x}/{y}.png"
   * Applied to the navigation map's style, re-applied automatically on
   * style changes; clear it (undefined/empty) to remove the overlay.
   * iOS: applied to the drop-in navigation map (not the pre-route
   * free-drive fallback map).
   */
  customRasterTileUrl?: string;

  /**
   * Style layer ID above which the custom raster layer is inserted.
   * If omitted (or the layer doesn't exist in the current style), the
   * raster layer is added on top.
   */
  customRasterAboveLayerId?: string;

  /**
   * Whether the built-in end-of-route screen ("You have arrived" + trip
   * rating stars) is shown when the final destination is reached.
   * Defaults to true (the SDK's own behavior). Set to false to arrive
   * silently — onArrival still fires either way.
   * iOS only: this screen is part of iOS's drop-in NavigationViewController;
   * Android's custom-built UI has no equivalent screen, so this prop is a
   * no-op there.
   */
  showEndOfRouteFeedback?: boolean;

  // ── Color customization ──────────────────────────────────────────────────

  /**
   * Background color of the turn-by-turn instruction banner, as a hex string
   * (e.g. "#1A73E8" or "1A73E8" — the "#" is optional). Uses the SDK's
   * official ManeuverViewOptions API. Day/light-mode variant — the
   * component automatically switches to `maneuverBackgroundColorNight`
   * between 8pm and 6am local time (Android) or via the system's day/night
   * style switching (iOS).
   */
  maneuverBackgroundColorDay?: string;

  /**
   * Same as `maneuverBackgroundColorDay`, applied during night mode.
   * On iOS this is only re-read the next time a route is presented, not
   * instantly — see the iOS Color Customization note in the README.
   */
  maneuverBackgroundColorNight?: string;

  /**
   * Color of the turn arrow icon inside the instruction banner.
   * Android: build-time plugin option required for the visible color (see
   * README). iOS (5.0.4+): applied via the SDK's ManeuverView appearance
   * at route presentation time.
   */
  maneuverTurnIconColor?: string;

  /**
   * Whether the ETA/duration/distance bottom bar is shown once navigation
   * starts. Defaults to true.
   * Android: applied live at any time.
   * iOS (5.0.4+): swaps the drop-in's bottom banner (ETA bar + cancel
   * button) for an empty one — construction-time, so changing it while
   * navigation is already active applies on the next route presentation.
   */
  showEta?: boolean;

  /**
   * Background color of the bottom ETA/duration/distance bar.
   * Android: applied live. iOS (5.0.4+): applied via the SDK's
   * BottomBannerView appearance at route presentation time.
   */
  etaBarBackgroundColor?: string;

  /**
   * Text color for the ETA time/duration/distance labels in the bottom
   * bar. Android: applied live. iOS (5.0.4+): applied via the SDK's
   * label appearances at route presentation time (also overrides the
   * SDK's traffic-based ETA coloring so the color stays uniform).
   */
  etaTextColor?: string;

  /**
   * Color of the mute/overview/recenter icon buttons (default state).
   * Android: applied live. iOS (5.0.4+): tints the drop-in's floating
   * buttons via the SDK's FloatingButton appearance at route
   * presentation time.
   */
  iconButtonColor?: string;

  /**
   * Color of the mute icon when voice guidance is muted.
   * Android: applied live. iOS (5.0.4+): tints the drop-in's mute
   * floating button when muted (SDK default button layout assumed:
   * overview, mute, feedback — per the SDK's own documentation).
   */
  iconButtonMutedColor?: string;

  /**
   * Tints the default location puck icon (the arrow/marker showing your
   * position and heading on the map — distinct from `maneuverTurnIconColor`,
   * which is inside the instruction banner). Ignored if
   * `navigationPuckImagePath` or `navigationPuck3DModelPath` is set.
   * On iOS, tints a system symbol rather than Mapbox's own default icon
   * shape (no confirmed public API for that exact asset on iOS).
   */
  navigationPuckColor?: string;

  /**
   * Replaces the puck icon with a local image (a `file://` URI or absolute
   * path — resolve bundled assets via `expo-asset` first). Never tinted,
   * even if `navigationPuckColor` is also set. Falls back to the color/
   * default icon if the file can't be loaded. Takes priority over
   * `navigationPuckColor`.
   */
  navigationPuckImagePath?: string;

  /**
   * Replaces the 2D puck with a 3D model (`.glb`/`.gltf`) — a local path,
   * `asset://name.glb` (Android bundled assets), or a full URL (both
   * platforms). Takes priority over both puck props above. Falls back to
   * the 2D puck if the model fails to load.
   */
  navigationPuck3DModelPath?: string;

  /**
   * Position of the speed limit panel. Defaults to "bottomLeft" (matches
   * the original, pre-existing position — no change unless set).
   * Android only — not yet implemented on iOS. "topRight" uses a wider
   * margin to clear the mute/overview/recenter buttons already occupying
   * that corner; verify visually on your target devices before relying on
   * it.
   */
  speedLimitPosition?: 'bottomLeft' | 'bottomRight' | 'topLeft' | 'topRight';

  // ── Route loading screen (5.1.4) ─────────────────────────────────────────

  /**
   * When true, covers the view with a native loading screen (opaque
   * background + spinner) from mount until the first route is actually
   * drawn — instead of showing the bare map first and the route popping
   * in a moment later. Dismissed with a short fade on the first
   * successful route, or on the first failure (so your own error UI is
   * never hidden behind it). One-shot per mounted view: later route
   * refetches (prop changes, reroutes) happen over the live map without
   * re-showing it. Defaults to false (existing behavior unchanged).
   * Ignored when `loadingScreen` is provided.
   */
  showRouteLoadingOverlay?: boolean;

  /**
   * Background color of the native route loading screen, as a hex string
   * (e.g. "#1E2433", the default). Only used with
   * `showRouteLoadingOverlay`; the spinner itself is always white.
   */
  loadingOverlayColor?: string;

  /**
   * Your own React Native component to show as the route loading screen,
   * instead of the built-in native one — e.g. your app's branded splash.
   * Rendered above the map, filling the view, until the first
   * onRoutesReady or onRoutesFailed event (your own handlers still fire
   * normally). Takes precedence over `showRouteLoadingOverlay`. Safe to
   * pass conditionally / compute from state that isn't ready on the very
   * first render — the underlying native map is never remounted because
   * of this prop, regardless of when or how often its presence changes.
   */
  loadingScreen?: React.ReactNode;

  // ── Event callbacks ──────────────────────────────────────────────────────

  onRouteProgressChanged?: (event: { nativeEvent: RouteProgressEvent }) => void;
  onRoutesReady?: (event: { nativeEvent: RoutesReadyEvent }) => void;
  onNavigationFinished?: (event: { nativeEvent: {} }) => void;
  onNavigationCancelled?: (event: { nativeEvent: {} }) => void;
  /**
   * Route calculation failed or was rejected. `message` is English-only
   * debugging text — do not display it to users directly. When `code` is
   * present, switch on it to show your own localized message:
   * - "same_location": all requested waypoints are within 25 m of each
   *   other (the driver is already at the destination) — no route was
   *   requested. Round trips via a distant point are NOT affected.
   * `code` is absent for ordinary routing failures (network errors,
   * no route found, etc.), whose `message` comes from the SDK.
   */
  onRoutesFailed?: (event: { nativeEvent: { message: string; code?: string } }) => void;
  onArrival?: (event: { nativeEvent: {} }) => void;

  /**
   * Fired when the user taps the turn-by-turn instruction banner.
   * Use this to open a bottom sheet / modal listing every upcoming step
   * of the route, including lane guidance data where available.
   */
  onManeuverBannerPressed?: (event: { nativeEvent: ManeuverBannerPressedEvent }) => void;

  style?: ViewStyle;
}

export interface RouteProgressEvent {
  distanceRemaining: number;
  durationRemaining: number;
  distanceTraveled: number;
  fractionTraveled: number;
  currentStepDistanceRemaining: number;
}

export interface RoutesReadyEvent {
  routeCount: number;
  distanceMeters: number;
  durationSeconds: number;
}

/** A single lane's directional options at an upcoming intersection. */
export interface LaneInstruction {
  /** Whether this lane is the recommended lane for the upcoming maneuver. */
  active: boolean;
  /** Directions this lane allows, e.g. ["straight"], ["left"], ["straight", "right"]. */
  directions: string[];
}

/** One step (maneuver) along the route, used for the full-route steps list. */
export interface RouteStep {
  /** Human-readable instruction text, e.g. "Turn left onto Main St". */
  instruction: string;
  /** Distance in metres for this step. */
  distanceMeters: number;
  /** Duration in seconds for this step. */
  durationSeconds: number;
  /** Directions API maneuver type, e.g. "turn", "merge", "roundabout". */
  maneuverType: string;
  /** Directions API maneuver modifier, e.g. "left", "right", "straight". */
  maneuverModifier: string;
  /** Name of the road for this step, if available. */
  roadName: string;
  /** Lane guidance for this step's upcoming maneuver, if available. */
  laneInstructions: LaneInstruction[];
}

export interface ManeuverBannerPressedEvent {
  steps: RouteStep[];
}

// ─────────────────────────────────────────────────────────────────────────────
// Native view
// ─────────────────────────────────────────────────────────────────────────────

const NativeView = requireNativeViewManager('ExpoMapboxNavigation');

/**
 * MapboxNavigationView
 *
 * Renders the Mapbox Drop-In Navigation UI inside your Expo/React Native app.
 *
 * @example
 * ```tsx
 * <MapboxNavigationView
 *   style={{ flex: 1 }}
 *   coordinates={[
 *     { latitude: 48.8566, longitude: 2.3522 },
 *     { latitude: 51.5074, longitude: -0.1278 },
 *   ]}
 *   voiceUnits="metric"
 *   language="fr"
 *   onArrival={() => console.log('Arrived!')}
 * />
 * ```
 */
export function MapboxNavigationView(props: MapboxNavigationViewProps) {
  const { loadingScreen, ...nativeProps } = props;
  // One-shot, mirroring the native overlay's contract exactly: visible
  // until the FIRST route success or failure, never re-shown afterwards
  // for this mounted instance (later refetches happen over the live map).
  const [routeLoading, setRouteLoading] = React.useState(true);

  // Stable identities across renders — no functional requirement forces
  // this (Expo Modules event props don't need referential stability), but
  // there's no reason to hand the native view a new function every render
  // either, and it removes one more variable from the investigation below.
  const handleRoutesReady = React.useCallback(
    (event: { nativeEvent: RoutesReadyEvent }) => {
      setRouteLoading(false);
      props.onRoutesReady?.(event);
    },
    [props.onRoutesReady],
  );
  const handleRoutesFailed = React.useCallback(
    (event: { nativeEvent: { message: string; code?: string } }) => {
      setRouteLoading(false);
      props.onRoutesFailed?.(event);
    },
    [props.onRoutesFailed],
  );

  const hasCustomLoadingScreen = loadingScreen != null;

  // CRITICAL (root-caused via a real-device adb logcat capture showing
  // THREE separate native ExpoMapboxNavigationView instances constructed
  // within ~300ms of each other during a single logical mount, one still
  // fully attached when the next one's init{} ran): the returned tree's
  // SHAPE must never depend on `loadingScreen`'s nullness. The previous
  // implementation returned a bare <NativeView> when loadingScreen was
  // null/undefined, and a <View><NativeView/>...</View> wrapper otherwise.
  // If a consumer's loadingScreen nullness differs between THIS
  // component's own first few renders (e.g. computed from state that
  // settles a moment after mount — the same class of instability that
  // commonly delays `coordinates` too), the root element TYPE changes
  // between renders, and React fully unmounts the old native view and
  // mounts a brand new one to match. MapboxNavigationProvider (the SDK's
  // own singleton, verified verbatim in its source) unconditionally
  // destroys whatever MapboxNavigation instance existed before on every
  // setupNavigation() call — so the still-attached OLD view's instance
  // gets destroyed out from under it the moment the NEW view's native
  // init{} runs, before the OLD view's own onDetachedFromWindow() ever
  // gets a chance to run first. That is exactly the observed real-device
  // crash ("This instance of MapboxNavigation is destroyed" + a Fabric
  // "Cannot remove child ... childCount may be incorrect" from the
  // resulting view-hierarchy corruption). Fixed by ALWAYS returning the
  // same wrapping <View> — only the conditionally-rendered SIBLING (the
  // loading screen itself) ever changes, an ordinary, safe React update
  // that never touches the NativeView's own position or type. The extra
  // <View> is a single flex:1 passthrough — no visible or behavioral
  // difference for consumers who never set `loadingScreen` at all.
  return (
    <View style={[styles.fullSize, props.style]}>
      <NativeView
        {...nativeProps}
        // The custom screen replaces the built-in native overlay entirely
        // when provided; otherwise nativeProps' own showRouteLoadingOverlay
        // (if set) passes through untouched.
        showRouteLoadingOverlay={
          hasCustomLoadingScreen ? false : props.showRouteLoadingOverlay
        }
        onRoutesReady={hasCustomLoadingScreen ? handleRoutesReady : props.onRoutesReady}
        onRoutesFailed={hasCustomLoadingScreen ? handleRoutesFailed : props.onRoutesFailed}
        style={StyleSheet.absoluteFill}
      />
      {hasCustomLoadingScreen && routeLoading ? (
        // Default pointerEvents (auto): blocks touches from reaching the
        // map underneath while loading, same as the native overlay.
        <View style={styles.loadingScreenContainer}>{loadingScreen}</View>
      ) : null}
    </View>
  );
}

const styles = StyleSheet.create({
  fullSize: { flex: 1 },
  loadingScreenContainer: {
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    bottom: 0,
  },
});

export default MapboxNavigationView;
