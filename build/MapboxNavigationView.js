"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.default = MapboxNavigationView;
const react_1 = __importDefault(require("react"));
const expo_modules_core_1 = require("expo-modules-core");
const NativeView = (0, expo_modules_core_1.requireNativeViewManager)('ExpoMapboxNavigation');
/**
 * MapboxNavigationView
 *
 * Composant de navigation Mapbox turn-by-turn pour Expo SDK 53+.
 * Requiert Expo Dev Client (incompatible avec Expo Go).
 *
 * @example
 * ```tsx
 * <MapboxNavigationView
 *   style={{ flex: 1 }}
 *   coordinates={[
 *     { latitude: 48.8566, longitude: 2.3522 },   // Paris (départ)
 *     { latitude: 45.7640, longitude: 4.8357 },   // Lyon (waypoint)
 *     { latitude: 43.2965, longitude: 5.3698 },   // Marseille (destination)
 *   ]}
 *   locale="fr"
 *   vehicleMaxHeight={5.0}
 *   vehicleMaxWidth={2.5}
 *   onRouteProgressChanged={(event) => {
 *     const { distanceRemaining, durationRemaining } = event.nativeEvent;
 *     console.log(`${(distanceRemaining / 1000).toFixed(1)} km restants`);
 *   }}
 *   onFinalDestinationArrival={() => console.log('Arrivé !')}
 *   onCancelNavigation={() => navigation.goBack()}
 * />
 * ```
 */
function MapboxNavigationView(props) {
    return (<NativeView 
    // Valeurs par défaut
    mute={false} useRouteMatchingApi={false} disableAlternativeRoutes={false} showsEndOfRouteFeedback={false} waypointIndices={[]} routeExcludeList={[]} {...props}/>);
}
