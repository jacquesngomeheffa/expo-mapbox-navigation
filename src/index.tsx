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

  // ── Event callbacks ──────────────────────────────────────────────────────

  onRouteProgressChanged?: (event: { nativeEvent: RouteProgressEvent }) => void;
  onRoutesReady?: (event: { nativeEvent: RoutesReadyEvent }) => void;
  onNavigationFinished?: (event: { nativeEvent: {} }) => void;
  onNavigationCancelled?: (event: { nativeEvent: {} }) => void;
  onRoutesFailed?: (event: { nativeEvent: { message: string } }) => void;
  onArrival?: (event: { nativeEvent: {} }) => void;

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
