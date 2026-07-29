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
    // Stable identities across renders — no functional requirement forces
    // this (Expo Modules event props don't need referential stability), but
    // there's no reason to hand the native view a new function every render
    // either, and it removes one more variable from the investigation below.
    const handleRoutesReady = react_1.default.useCallback((event) => {
        var _a;
        setRouteLoading(false);
        (_a = props.onRoutesReady) === null || _a === void 0 ? void 0 : _a.call(props, event);
    }, [props.onRoutesReady]);
    const handleRoutesFailed = react_1.default.useCallback((event) => {
        var _a;
        setRouteLoading(false);
        (_a = props.onRoutesFailed) === null || _a === void 0 ? void 0 : _a.call(props, event);
    }, [props.onRoutesFailed]);
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
    return (react_1.default.createElement(react_native_1.View, { style: [styles.fullSize, props.style] },
        react_1.default.createElement(NativeView, { ...nativeProps, 
            // The custom screen replaces the built-in native overlay entirely
            // when provided; otherwise nativeProps' own showRouteLoadingOverlay
            // (if set) passes through untouched.
            showRouteLoadingOverlay: hasCustomLoadingScreen ? false : props.showRouteLoadingOverlay, onRoutesReady: hasCustomLoadingScreen ? handleRoutesReady : props.onRoutesReady, onRoutesFailed: hasCustomLoadingScreen ? handleRoutesFailed : props.onRoutesFailed, style: react_native_1.StyleSheet.absoluteFill }),
        hasCustomLoadingScreen && routeLoading ? (
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
