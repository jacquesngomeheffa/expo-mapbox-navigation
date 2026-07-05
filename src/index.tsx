import {
  requireNativeViewManager,
  NativeModulesProxy,
  EventEmitter,
} from 'expo-modules-core';
import React from 'react';
import { ViewStyle, StyleSheet } from 'react-native';

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
   * When true, uses the Mapbox Map Matching API instead of the
   * standard Directions API. Useful when you want a route that
   * exactly follows the given coordinates.
   */
  useMapMatching?: boolean;

  /**
   * URL template for a custom raster tile overlay.
   * Must include {x}, {y}, {z} placeholders.
   * Example: "https://tile.openstreetmap.org/{z}/{x}/{y}.png"
   */
  customRasterTileUrl?: string;

  /** Map layer ID above which the custom raster layer is placed. */
  customRasterAboveLayerId?: string;

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

  /** Color of the turn arrow icon inside the instruction banner. Android only — not yet implemented on iOS (see README). */
  maneuverTurnIconColor?: string;

  /**
   * Whether the ETA/duration/distance bar is shown once navigation starts.
   * Defaults to true.
   */
  showEta?: boolean;

  /** Background color of the bottom ETA/duration/distance bar. */
  etaBarBackgroundColor?: string;

  /** Text color used for the ETA time and duration in the bottom bar. */
  etaTextColor?: string;

  /** Color of the mute/overview/recenter icon buttons (default state). */
  iconButtonColor?: string;

  /** Color of the mute icon when voice guidance is muted. */
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

  // ── Event callbacks ──────────────────────────────────────────────────────

  onRouteProgressChanged?: (event: { nativeEvent: RouteProgressEvent }) => void;
  onRoutesReady?: (event: { nativeEvent: RoutesReadyEvent }) => void;
  onNavigationFinished?: (event: { nativeEvent: {} }) => void;
  onNavigationCancelled?: (event: { nativeEvent: {} }) => void;
  onRoutesFailed?: (event: { nativeEvent: { message: string } }) => void;
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
  return (
    <NativeView
      {...props}
      style={[styles.fullSize, props.style]}
    />
  );
}

const styles = StyleSheet.create({
  fullSize: { flex: 1 },
});

export default MapboxNavigationView;
