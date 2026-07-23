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
    const { loadingScreen, ...nativeProps } = props;
    // One-shot, mirroring the native overlay's contract exactly: visible
    // until the FIRST route success or failure, never re-shown afterwards
    // for this mounted instance (later refetches happen over the live map).
    const [routeLoading, setRouteLoading] = react_1.default.useState(true);
    // No custom loading screen — render the native view directly, exactly
    // as every version before 5.1.4 did (zero tree-shape change for
    // existing consumers; the native showRouteLoadingOverlay prop covers
    // the built-in variant without any JS involvement).
    if (loadingScreen == null) {
        return (react_1.default.createElement(NativeView, { ...nativeProps, style: [styles.fullSize, props.style] }));
    }
    const handleRoutesReady = (event) => {
        var _a;
        setRouteLoading(false);
        (_a = props.onRoutesReady) === null || _a === void 0 ? void 0 : _a.call(props, event);
    };
    const handleRoutesFailed = (event) => {
        var _a;
        setRouteLoading(false);
        (_a = props.onRoutesFailed) === null || _a === void 0 ? void 0 : _a.call(props, event);
    };
    return (react_1.default.createElement(react_native_1.View, { style: [styles.fullSize, props.style] },
        react_1.default.createElement(NativeView, { ...nativeProps, 
            // The custom screen replaces the built-in native overlay entirely.
            showRouteLoadingOverlay: false, onRoutesReady: handleRoutesReady, onRoutesFailed: handleRoutesFailed, style: react_native_1.StyleSheet.absoluteFill }),
        routeLoading ? (
        // Default pointerEvents (auto): blocks touches from reaching the
        // map underneath while loading, same as the native overlay.
        react_1.default.createElement(react_native_1.View, { style: styles.loadingScreenContainer }, loadingScreen)) : null));
}
const styles = react_native_1.StyleSheet.create({
    fullSize: { flex: 1 },
    loadingScreenContainer: {
        position: 'absolute',
        top: 0,
        left: 0,
        right: 0,
        bottom: 0,
    },
});
exports.default = MapboxNavigationView;
