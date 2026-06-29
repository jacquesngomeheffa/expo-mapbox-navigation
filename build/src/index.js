"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.MapboxNavigationView = MapboxNavigationView;
const expo_modules_core_1 = require("expo-modules-core");
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
// ─────────────────────────────────────────────────────────────────────────────
// Native view
// ─────────────────────────────────────────────────────────────────────────────
const NativeView = (0, expo_modules_core_1.requireNativeViewManager)('ExpoMapboxNavigation');
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
function MapboxNavigationView(props) {
    return (react_1.default.createElement(NativeView, { ...props, style: [styles.fullSize, props.style] }));
}
const styles = react_native_1.StyleSheet.create({
    fullSize: { flex: 1 },
});
exports.default = MapboxNavigationView;
