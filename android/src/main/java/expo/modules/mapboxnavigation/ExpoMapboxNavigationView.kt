package expo.modules.mapboxnavigation

import android.annotation.SuppressLint
import android.content.Context
import android.content.res.Resources
import android.graphics.BitmapFactory
import android.net.Uri
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.Path
import android.graphics.PorterDuff
import android.graphics.PorterDuffXfermode
import android.graphics.RectF
import android.graphics.drawable.BitmapDrawable
import android.graphics.drawable.GradientDrawable
import androidx.core.content.ContextCompat
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import android.view.ContextThemeWrapper
import java.io.File
import android.util.Log
import android.view.Gravity
import android.view.View
import android.widget.FrameLayout
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import com.mapbox.api.directions.v5.models.RouteOptions
import com.mapbox.common.location.Location
import com.mapbox.geojson.Point
import com.mapbox.maps.CameraOptions
import com.mapbox.maps.Style
import com.mapbox.maps.extension.style.layers.addLayer
import com.mapbox.maps.extension.style.layers.addLayerAbove
import com.mapbox.maps.extension.style.layers.generated.rasterLayer
import com.mapbox.maps.extension.style.layers.properties.generated.IconAnchor
import com.mapbox.maps.extension.style.sources.addSource
import com.mapbox.maps.extension.style.sources.generated.rasterSource
import com.mapbox.navigation.base.ExperimentalPreviewMapboxNavigationAPI
import com.mapbox.navigation.core.mapmatching.MapMatchingAPICallback
import com.mapbox.navigation.core.mapmatching.MapMatchingFailure
import com.mapbox.navigation.core.mapmatching.MapMatchingOptions
import com.mapbox.navigation.core.mapmatching.MapMatchingSuccessfulResult
import com.mapbox.maps.plugin.annotation.annotations
import com.mapbox.maps.plugin.annotation.generated.PointAnnotationManager
import com.mapbox.maps.plugin.annotation.generated.PointAnnotationOptions
import com.mapbox.maps.plugin.annotation.generated.createPointAnnotationManager
import com.mapbox.maps.EdgeInsets
import com.mapbox.maps.ImageHolder
import com.mapbox.maps.MapView
import com.mapbox.maps.plugin.LocationPuck2D
import com.mapbox.maps.plugin.LocationPuck3D
import com.mapbox.maps.plugin.PuckBearing
import com.mapbox.maps.plugin.animation.camera
import com.mapbox.maps.plugin.locationcomponent.location
import com.mapbox.navigation.base.TimeFormat
import com.mapbox.navigation.base.extensions.applyDefaultNavigationOptions
import com.mapbox.navigation.base.formatter.DistanceFormatterOptions
import com.mapbox.navigation.base.formatter.UnitType
import com.mapbox.navigation.base.options.NavigationOptions
import com.mapbox.navigation.base.route.NavigationRoute
import com.mapbox.navigation.base.route.NavigationRouterCallback
import com.mapbox.navigation.base.route.RouterFailure
import com.mapbox.navigation.base.route.RouterOrigin
import com.mapbox.navigation.base.speed.model.SpeedLimitInfo
import com.mapbox.navigation.base.speed.model.SpeedLimitSign
import com.mapbox.navigation.base.speed.model.SpeedUnit
import com.mapbox.navigation.core.MapboxNavigation
import com.mapbox.navigation.core.MapboxNavigationProvider
import com.mapbox.navigation.core.directions.session.RoutesObserver
import com.mapbox.navigation.core.directions.session.RoutesUpdatedResult
import com.mapbox.navigation.core.formatter.MapboxDistanceFormatter
import com.mapbox.navigation.core.trip.session.LocationMatcherResult
import com.mapbox.navigation.core.trip.session.LocationObserver
import com.mapbox.navigation.core.trip.session.RouteProgressObserver
import com.mapbox.navigation.core.trip.session.VoiceInstructionsObserver
import com.mapbox.navigation.tripdata.maneuver.api.MapboxManeuverApi
import com.mapbox.navigation.tripdata.progress.api.MapboxTripProgressApi
import com.mapbox.navigation.tripdata.progress.model.DistanceRemainingFormatter
import com.mapbox.navigation.tripdata.progress.model.EstimatedTimeToArrivalFormatter
import com.mapbox.navigation.tripdata.progress.model.TimeRemainingFormatter
import com.mapbox.navigation.tripdata.progress.model.TripProgressUpdateFormatter
import com.mapbox.navigation.tripdata.speedlimit.api.MapboxSpeedInfoApi
import com.mapbox.navigation.ui.components.maneuver.model.ManeuverViewOptions
import com.mapbox.navigation.ui.components.maneuver.view.MapboxManeuverView
import com.mapbox.navigation.ui.components.speedlimit.view.MapboxSpeedInfoView
import com.mapbox.navigation.ui.components.tripprogress.view.MapboxTripProgressView
import com.mapbox.navigation.ui.maps.NavigationStyles
import com.mapbox.navigation.ui.maps.camera.NavigationCamera
import com.mapbox.navigation.ui.maps.camera.state.NavigationCameraState
import com.mapbox.navigation.ui.maps.camera.data.MapboxNavigationViewportDataSource
import com.mapbox.navigation.ui.maps.camera.transition.NavigationCameraTransitionOptions
import com.mapbox.navigation.ui.maps.location.NavigationLocationProvider
import com.mapbox.navigation.ui.maps.route.arrow.api.MapboxRouteArrowApi
import com.mapbox.navigation.ui.maps.route.arrow.api.MapboxRouteArrowView
import com.mapbox.navigation.ui.maps.route.arrow.model.RouteArrowOptions
import com.mapbox.navigation.ui.maps.route.line.api.MapboxRouteLineApi
import com.mapbox.navigation.ui.maps.route.line.api.MapboxRouteLineView
import com.mapbox.navigation.ui.maps.route.line.model.MapboxRouteLineApiOptions
import com.mapbox.navigation.ui.maps.route.line.model.MapboxRouteLineViewOptions
import com.mapbox.navigation.voice.api.MapboxSpeechApi
import com.mapbox.navigation.voice.api.MapboxVoiceInstructionsPlayer
import com.mapbox.navigation.voice.model.SpeechAnnouncement
import com.mapbox.navigation.voice.model.SpeechError
import com.mapbox.navigation.voice.model.SpeechValue
import com.mapbox.navigation.voice.model.SpeechVolume
import expo.modules.kotlin.AppContext
import expo.modules.kotlin.viewevent.EventDispatcher
import expo.modules.kotlin.views.ExpoView
import java.util.Calendar
import java.util.Locale
import kotlin.math.roundToInt

private const val TAG = "ExpoMapboxNavigation"

// Parses a hex color string safely, tolerating a missing leading "#" —
// Android's own Color.parseColor() REQUIRES the "#" and throws
// IllegalArgumentException without it, which every call site already
// wraps in try/catch — meaning a hex string passed from JS without a
// leading "#" (e.g. "1E2433" instead of "#1E2433") was silently failing
// and falling back to whatever default applied, with no visible error.
// Used for every color prop that comes from JS/props (maneuver colors,
// puck color, ETA bar colors, icon button colors) — not for this
// package's own hardcoded, already-correctly-formatted default values.
private fun parseColorSafe(hex: String): Int? {
    val normalized = if (hex.startsWith("#")) hex else "#$hex"
    return try {
        Color.parseColor(normalized)
    } catch (e: Exception) {
        Log.e(TAG, "Invalid color string: \"$hex\" (${e.message})")
        null
    }
}

// Degenerate-route guard constants (5.0.1) - kept in exact parity with the
// iOS side (ExpoMapboxNavigationView.swift declares the same threshold and
// the same message verbatim). See the guard inside fetchRoutes() for the
// full rationale.
private const val DEGENERATE_ROUTE_THRESHOLD_METERS = 25f
private const val DEGENERATE_ROUTE_MESSAGE =
    "Origin and destination are the same location (all waypoints within 25 m) - nothing to navigate"

// Native steps panel (5.0.1): hard cap on the panel's height, as a fraction
// of the navigation view's own height. Product requirement: the list must
// not extend all the way to the bottom of the screen.
private const val STEPS_PANEL_MAX_HEIGHT_FRACTION = 0.86f

// Degenerate-route camera (5.0.2): street-level zoom used to center the map
// on the driver when the whole trip is a single point and no navigation
// session will ever start (so the usual first-location camera centering,
// which depends on the trip session's location updates, never triggers).
private const val DEGENERATE_ROUTE_CAMERA_ZOOM = 15.0

// Custom raster overlay (5.0.4, zero-dead-props phase) - stable ids for the
// injected source/layer so re-application on style reloads is idempotent.
private const val CUSTOM_RASTER_SOURCE_ID = "expo_mapbox_navigation_raster_source"
private const val CUSTOM_RASTER_LAYER_ID = "expo_mapbox_navigation_raster_layer"

// Style-load failure recovery (5.0.9) - bounded so a persistently failing
// environment (offline, style host unreachable) degrades to a finite
// number of attempts, not an infinite retry loop.
private const val MAX_STYLE_LOAD_RETRIES = 3
private const val STYLE_LOAD_RETRY_DELAY_MS = 1500L

class ExpoMapboxNavigationView(context: Context, appContext: AppContext) :
    ExpoView(context, appContext) {

    // ── EventDispatchers ──────────────────────────────────────────────────────
    private val onRouteProgressChanged by EventDispatcher()
    private val onRoutesReady by EventDispatcher()
    private val onNavigationFinished by EventDispatcher()
    private val onNavigationCancelled by EventDispatcher()
    private val onRoutesFailed by EventDispatcher()
    private val onArrival by EventDispatcher()
    // Feature: emits the full list of turn-by-turn steps when the instruction
    // banner is tapped, so the RN layer can render a steps list (bottom
    // sheet/modal). Payload: { steps: [{ instruction, distanceMeters,
    // maneuverType, maneuverModifier, laneInstructions }] }
    private val onManeuverBannerPressed by EventDispatcher()

    // ── Views ─────────────────────────────────────────────────────────────────
    private val mapView: MapView = MapView(context)
    private var maneuverView: MapboxManeuverView? = null
    private var speedInfoView: MapboxSpeedInfoView? = null
    private var tripProgressView: MapboxTripProgressView? = null
    private var tvEtaTime: TextView? = null
    private var tvDuration: TextView? = null
    private var tvDistance: TextView? = null
    // Latest system bar insets (status bar / navigation bar), kept in sync via
    // the WindowInsets listener set up in buildUI(). Read by
    // applySpeedLimitPosition() and the ETA bar's own margin calculation —
    // real devices reserve real screen space for the navigation bar
    // (3-button or gesture), which the emulator's own default configuration
    // doesn't always reproduce, meaning bottom/top-anchored overlay elements
    // positioned without accounting for this can end up rendered fully or
    // partially UNDER the system bar on real hardware despite being
    // genuinely marked visible — invisible in practice, not in code. Also
    // relevant regardless of any specific device: edge-to-edge display is
    // becoming mandatory (not optional) starting Android 15+.
    private var lastSystemBarInsets: androidx.core.graphics.Insets = androidx.core.graphics.Insets.NONE

    // ─────────────────────────────────────────────────────────────────────────
    // FIX: ETA bar invisible on some real devices even with the WindowInsets
    // listener (set up in buildUI()) already in place.
    //
    // Root cause, confirmed against Expo's own SDK 53 changelog (not a
    // guess): starting Android 15, edge-to-edge is enforced for apps
    // targeting API 35 — content can legitimately draw behind the system
    // navigation bar. Starting Android 16 specifically, the opt-out
    // attribute (windowOptOutEdgeToEdgeEnforcement) is disabled outright —
    // there is no way for an app to avoid this, regardless of its own
    // configuration. On devices/OS versions predating this (e.g. Android
    // 13), the system bar reserves real screen space the traditional way,
    // so insets being Insets.NONE there was always correct — nothing to
    // fix. On Android 15+, that's no longer true, and this view depends on
    // actually receiving a WindowInsets dispatch to know it.
    //
    // The existing ViewCompat.setOnApplyWindowInsetsListener(root) in
    // buildUI() only works if that dispatch actually reaches this nested
    // native view. On React Native's New Architecture (Fabric), with
    // react-native-safe-area-context present in the host app (as it is
    // here), insets are commonly consumed higher up the RN view tree —
    // by SafeAreaProvider or RN's own root view handling — before ever
    // reaching a custom native view nested inside it. When that happens,
    // lastSystemBarInsets stays stuck at Insets.NONE for this view's
    // entire lifetime, and the ETA bar's bottom margin never adjusts —
    // positioning it under the system navigation bar on hardware where
    // content now genuinely draws edge-to-edge.
    //
    // fetchSystemBarInsetsDirectly() sidesteps this by reading insets
    // straight from the Activity's own decorView, independently of
    // whatever does or doesn't reach this view via RN's own dispatch
    // chain. Called once in buildUI() (see applySystemBarInsets below) to
    // seed a correct value immediately, in addition to — not instead of —
    // the existing listener, which still keeps the value in sync for
    // later changes (rotation, nav bar show/hide) on devices where
    // dispatch does reach this view normally. Returns Insets.NONE (a
    // no-op, identical to this package's previous behavior) if `context`
    // isn't an Activity or no insets are available yet — never throws.
    // ─────────────────────────────────────────────────────────────────────────
    private fun fetchSystemBarInsetsDirectly(): androidx.core.graphics.Insets {
        val activity = findActivity(context) ?: return androidx.core.graphics.Insets.NONE
        val decorView = activity.window?.decorView ?: return androidx.core.graphics.Insets.NONE
        val insets = ViewCompat.getRootWindowInsets(decorView) ?: return androidx.core.graphics.Insets.NONE
        return insets.getInsets(WindowInsetsCompat.Type.systemBars())
    }

    // Resolves the actual Activity out of a possibly-wrapped Context.
    // NOT a direct `context as? Activity` cast: the Context handed to an
    // Expo/RN native view is frequently a ReactContext or similar
    // ContextWrapper — itself not an Activity, even though it wraps one —
    // so a direct cast would silently return null on exactly the setups
    // this fix targets, defeating the whole point. Unwraps one
    // ContextWrapper layer at a time (the standard, well-known Android
    // pattern for this) until an actual Activity is found or the chain is
    // exhausted. Returns null (never throws) if no Activity is found
    // anywhere in the chain.
    private fun findActivity(ctx: Context): android.app.Activity? {
        var current = ctx
        while (current is android.content.ContextWrapper) {
            if (current is android.app.Activity) return current
            current = current.baseContext
        }
        return current as? android.app.Activity
    }

    // Single shared update path for applying a given set of system bar
    // insets to both the ETA bar's bottom margin and the speed limit
    // panel's position — used by both fetchSystemBarInsetsDirectly()'s
    // one-time seed call and the ongoing WindowInsets listener, so both
    // mechanisms stay perfectly consistent with each other (no duplicated
    // or divergent logic between the two).
    private fun applySystemBarInsets(insets: androidx.core.graphics.Insets) {
        lastSystemBarInsets = insets
        etaBar?.let { bar ->
            (bar.layoutParams as? FrameLayout.LayoutParams)?.let { lp ->
                lp.bottomMargin = lastSystemBarInsets.bottom
                bar.layoutParams = lp
            }
        }
        applySpeedLimitPosition()
    }

    private var etaBar: LinearLayout? = null
    private var btnMuteView: ImageView? = null
    private var btnOverviewView: ImageView? = null
    private var btnRecenterView: ImageView? = null
    private var sideButtons: LinearLayout? = null

    // ── Navigation APIs ───────────────────────────────────────────────────────
    private lateinit var navigationCamera: NavigationCamera
    private lateinit var viewportDataSource: MapboxNavigationViewportDataSource
    private lateinit var routeLineApi: MapboxRouteLineApi
    private lateinit var routeLineView: MapboxRouteLineView
    private lateinit var maneuverApi: MapboxManeuverApi
    private lateinit var tripProgressApi: MapboxTripProgressApi
    private lateinit var speedInfoApi: MapboxSpeedInfoApi
    private val routeArrowApi = MapboxRouteArrowApi()
    private lateinit var routeArrowView: MapboxRouteArrowView
    private val navigationLocationProvider = NavigationLocationProvider()
    private var mapboxNavigation: MapboxNavigation? = null

    // ── Voice APIs ────────────────────────────────────────────────────────────
    private lateinit var speechApi: MapboxSpeechApi
    private lateinit var voiceInstructionsPlayer: MapboxVoiceInstructionsPlayer

    // ── State ─────────────────────────────────────────────────────────────────
    private var isNightMode = false
    private var isMuted = false
    private var isOverviewMode = false
    private var firstLocationReceived = false
    // Guards applyPuckSettings() against running before
    // mapView.location.setLocationProvider() has been called — that call
    // happens inside setupNavigation()'s ASYNC mapboxMap.loadStyle(...)
    // callback (style loading is not instant), not synchronously during
    // init{}. If a puck-related prop setter fires before that callback has
    // run (a real possibility — Expo delivers props right after native
    // view creation, independently of when the map style finishes
    // loading), calling mapView.location.updateSettings{} that early would
    // race ahead of setLocationProvider on the same plugin instance. The
    // setter-triggered calls below skip re-applying until this is true;
    // the one-time initial call inside the loadStyle callback always runs
    // regardless and will already reflect whatever the current prop values
    // are by the time it fires.
    private var navigationSetupComplete = false
    // Bounded retry counter for style-load failures (5.0.9) - see
    // MAX_STYLE_LOAD_RETRIES / setupNavigation()'s subscribeMapLoadingError.
    private var styleLoadRetryCount = 0

    // ── Pixel density ─────────────────────────────────────────────────────────
    private val dp = context.resources.displayMetrics.density

    // ── Viewport padding — EXACT official Mapbox values from the camera guide ──
    // Previous bottom=300dp pushed the focal point too far up, causing
    // the puck to visibly jump/drift between location updates.
    // Official guide value: top=180, left=40, bottom=150, right=40
    // ── Viewport padding — Waze-style vehicle position ──────────────────────────
    // bottom=300dp pushes the focal point (and thus the vehicle puck) up to
    // roughly 30% from the bottom of the screen, matching Waze/Google Maps.
    // NOTE: this was briefly regressed to bottom=150dp (the "exact" value from
    // the Mapbox camera guide example) while investigating puck jitter — but
    // the jitter was actually caused by passing keyPoints to changePosition()
    // (see the LocationObserver fix below), NOT by this padding value. The
    // jitter fix and the Waze-style positioning are independent and both
    // needed; restoring bottom=300dp here does not reintroduce the jitter.
    // NAVIGATION ICON POSITION (5.1.0): the four insets below are individually
    // overridable via navigationViewportPadding{Top,Left,Bottom,Right} props
    // (each nullable, default null - see the prop declarations below). Left
    // as a `get()` (recomputed on every read, not `by lazy`/cached) so a
    // live prop change takes effect without needing this whole view to be
    // recreated. Defaults are the EXACT pre-5.1.0 hardcoded values -
    // unchanged unless a prop is explicitly set, per this package's
    // never-change-existing-behavior-silently rule.
    private val followingPadding: EdgeInsets
        get() = EdgeInsets(
            (navigationViewportPaddingTop ?: 180.0) * dp,
            (navigationViewportPaddingLeft ?: 40.0) * dp,
            (navigationViewportPaddingBottom ?: 300.0) * dp,
            (navigationViewportPaddingRight ?: 40.0) * dp
        )
    private val overviewPadding by lazy {
        EdgeInsets(140.0 * dp, 40.0 * dp, 120.0 * dp, 40.0 * dp)
    }

    // ── Props ─────────────────────────────────────────────────────────────────
    private var coordinates: List<Map<String, Double>> = emptyList()
    private var waypointIndices: List<Int>? = null
    private var language: String? = null
    private var voiceUnits: String? = null
    private var navigationProfile: String? = null
    private var excludeTypes: List<String>? = null
    private var mapStyle: String? = null
    private var mute: Boolean = false
    private var maxHeight: Double? = null
    private var maxWidth: Double? = null
    // Navigation icon (puck) position overrides (5.1.0) - each independently
    // optional, in dp. null (the default for all four) means "use this
    // package's existing Waze-style values" - see followingPadding's get()
    // above for the exact fallback numbers. Only `bottom` is the one most
    // consumers would ever touch (it's what pushes the puck up from the
    // true bottom edge), but all four are exposed for full flexibility,
    // matching the shape of the native EdgeInsets API these map to 1:1.
    private var navigationViewportPaddingTop: Double? = null
    private var navigationViewportPaddingLeft: Double? = null
    private var navigationViewportPaddingBottom: Double? = null
    private var navigationViewportPaddingRight: Double? = null
    private var useMapMatching: Boolean = false
    private var customRasterTileUrl: String? = null
    private var customRasterAboveLayerId: String? = null

    // ── Color customization props ────────────────────────────────────────────
    // Maneuver banner colors — verified against ManeuverViewOptions public API
    // (maneuverBackgroundColor, turnIconManeuver are real documented properties)
    private var maneuverBackgroundColorDay: String? = null
    private var maneuverBackgroundColorNight: String? = null
    private var maneuverTurnIconColor: String? = null
    // ETA bottom bar colors — fully custom view, safe to color freely
    private var etaBarBackgroundColor: String? = null
    // Explicit visibility control for the ETA/duration/distance bar.
    // Default true (matches existing automatic show/hide behavior driven by
    // showUI()/hideUI() — no regression for anyone not setting this). When
    // explicitly set to false, the ETA bar stays hidden even after
    // showUI() would otherwise reveal it.
    private var showEta: Boolean = true
    private var etaTextColor: String? = null
    // Custom icon button colors (mute, overview, recenter) — our own bitmaps
    private var iconButtonColor: String? = null
    private var iconButtonMutedColor: String? = null
    // Location puck (the arrow/icon showing the user's position and heading
    // on the map itself — distinct from maneuverTurnIconColor, which colors
    // the turn-direction icon inside the instruction banner, not the map).
    private var navigationPuckColor: String? = null
    // Speed limit panel position — one of "bottomLeft" (default, matches
    // the original hardcoded position), "bottomRight", "topLeft", "topRight".
    private var speedLimitPosition: String = "bottomLeft"
    // Local image path (file:// URI or absolute path) to fully replace the
    // default 2D puck icon. Takes precedence over navigationPuckColor —
    // color tinting is never applied to a custom image (avoids any risk of
    // that operation failing/crashing on an arbitrary user-supplied image).
    private var navigationPuckImagePath: String? = null
    // Local path (file:// URI, absolute path, or "asset://name.glb" for a
    // file bundled in Android's own assets/ folder) to a .glb/.gltf 3D
    // model, replacing the 2D puck entirely with a LocationPuck3D. Takes
    // precedence over both navigationPuckImagePath and navigationPuckColor
    // when set and valid — if the model fails to load for any reason, falls
    // back to the 2D puck (image or color, per the rules above) rather than
    // leaving the map without any puck at all, or crashing.
    private var navigationPuck3DModelPath: String? = null

    init {
        buildUI()
        initAPIs()
        setupNavigation()
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Draw icons programmatically (matching screenshot: speaker, route, arrow)
    // ─────────────────────────────────────────────────────────────────────────

    // ── Modern Google Maps / Waze style icons ──────────────────────────────────

    private fun drawSpeakerIcon(muted: Boolean): Bitmap {
        val size = (44 * dp).toInt()
        val bmp = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        val c = Canvas(bmp)
        val s = size.toFloat()
        val defaultColor = if (muted) "#5F6368" else "#1A73E8"
        val userColor = if (muted) iconButtonMutedColor else iconButtonColor
        val color = userColor?.let { parseColorSafe(it) } ?: Color.parseColor(defaultColor)
        val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
            this.color = color
            style = Paint.Style.FILL
        }
        // Speaker cone (rounded, modern Material style)
        val body = Path().apply {
            moveTo(s * 0.16f, s * 0.40f)
            lineTo(s * 0.34f, s * 0.40f)
            lineTo(s * 0.56f, s * 0.20f)
            quadTo(s * 0.60f, s * 0.17f, s * 0.60f, s * 0.22f)
            lineTo(s * 0.60f, s * 0.78f)
            quadTo(s * 0.60f, s * 0.83f, s * 0.56f, s * 0.80f)
            lineTo(s * 0.34f, s * 0.60f)
            lineTo(s * 0.16f, s * 0.60f)
            quadTo(s * 0.12f, s * 0.60f, s * 0.12f, s * 0.56f)
            lineTo(s * 0.12f, s * 0.44f)
            quadTo(s * 0.12f, s * 0.40f, s * 0.16f, s * 0.40f)
            close()
        }
        c.drawPath(body, paint)
        if (!muted) {
            paint.style = Paint.Style.STROKE
            paint.strokeWidth = s * 0.06f
            paint.strokeCap = Paint.Cap.ROUND
            c.drawArc(RectF(s*0.62f, s*0.34f, s*0.80f, s*0.66f), -45f, 90f, false, paint)
            c.drawArc(RectF(s*0.66f, s*0.22f, s*0.92f, s*0.78f), -40f, 80f, false, paint)
        } else {
            paint.style = Paint.Style.STROKE
            paint.strokeWidth = s * 0.065f
            paint.strokeCap = Paint.Cap.ROUND
            paint.color = Color.parseColor("#EA4335")
            c.drawLine(s*0.66f, s*0.34f, s*0.88f, s*0.66f, paint)
            c.drawLine(s*0.88f, s*0.34f, s*0.66f, s*0.66f, paint)
        }
        return bmp
    }

    private fun drawRouteOverviewIcon(): Bitmap {
        val size = (44 * dp).toInt()
        val bmp = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        val c = Canvas(bmp)
        val s = size.toFloat()
        val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
            color = iconButtonColor?.let { parseColorSafe(it) } ?: Color.parseColor("#1A73E8")
        }
        paint.style = Paint.Style.STROKE
        paint.strokeWidth = s * 0.07f
        paint.strokeJoin = Paint.Join.ROUND
        paint.strokeCap = Paint.Cap.ROUND
        // Outer diamond/map frame
        val frame = Path().apply {
            moveTo(s * 0.50f, s * 0.14f)
            lineTo(s * 0.84f, s * 0.34f)
            lineTo(s * 0.84f, s * 0.66f)
            lineTo(s * 0.50f, s * 0.86f)
            lineTo(s * 0.16f, s * 0.66f)
            lineTo(s * 0.16f, s * 0.34f)
            close()
        }
        c.drawPath(frame, paint)
        // Two horizontal fold lines (classic map icon)
        c.drawLine(s*0.36f, s*0.22f, s*0.36f, s*0.78f, paint)
        c.drawLine(s*0.64f, s*0.22f, s*0.64f, s*0.78f, paint)
        return bmp
    }

    private fun drawNavigationArrowIcon(): Bitmap {
        val size = (44 * dp).toInt()
        val bmp = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        val c = Canvas(bmp)
        val s = size.toFloat()
        val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
            color = iconButtonColor?.let { parseColorSafe(it) } ?: Color.parseColor("#1A73E8")
            style = Paint.Style.FILL
        }
        val arrow = Path().apply {
            moveTo(s * 0.50f, s * 0.12f)    // tip
            lineTo(s * 0.80f, s * 0.82f)    // bottom-right
            lineTo(s * 0.50f, s * 0.64f)    // notch center
            lineTo(s * 0.20f, s * 0.82f)    // bottom-left
            close()
        }
        c.drawPath(arrow, paint)
        return bmp
    }

    private fun makeIconButton(bitmap: Bitmap, onClick: () -> Unit): ImageView {
        val size = (56 * dp).toInt()
        val iv = ImageView(context).apply {
            setImageBitmap(bitmap)
            scaleType = ImageView.ScaleType.CENTER_INSIDE
            setPadding((8 * dp).toInt(), (8 * dp).toInt(), (8 * dp).toInt(), (8 * dp).toInt())
            background = GradientDrawable().apply {
                shape = GradientDrawable.OVAL
                setColor(Color.WHITE)
            }
            elevation = 6 * dp
            setOnClickListener { onClick() }
        }
        return iv
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Build UI
    // ─────────────────────────────────────────────────────────────────────────
    private fun buildUI() {
        val root = FrameLayout(context).apply {
            layoutParams = LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT)
        }
        // Class-level handle so overlays added later (native steps panel,
        // 5.0.1) can attach to / detach from this same FrameLayout.
        rootLayout = root

        // Full-screen map
        root.addView(mapView, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ))

        // ── ManeuverView — top ─────────────────────────────────────────────────
        // Colors are customizable via maneuverBackgroundColorDay,
        // maneuverBackgroundColorNight, maneuverTurnIconColor, and
        // laneGuidanceTurnIconColor props using the SDK's official
        // ManeuverViewOptions.Builder — confirmed public API
        // (maneuverBackgroundColor, turnIconManeuver, laneGuidanceTurnIconManeuver
        // are all real documented properties of ManeuverViewOptions).
        //
        // FIX (see setManeuverBackgroundColorDay/Night/TurnIconColor below):
        // this used to build ManeuverViewOptions inline, right here, using
        // whatever maneuverBackgroundColorDay/maneuverTurnIconColor held AT
        // THIS EXACT MOMENT — but buildUI() runs from init{}, which executes
        // before Expo/React Native has delivered ANY props to this view (props
        // always arrive via setter calls made AFTER the native view instance
        // already exists). So these were always still null here, and the
        // color never actually took effect, no matter what was set from JS.
        // createManeuverView() is now also called (to REBUILD, not just
        // build) from each color prop's setter and from
        // checkAndSwitchDayNight() whenever day/night actually flips, so the
        // correct color is always applied once it's actually known/changes.
        val mv = createManeuverView()
        root.addView(mv as View, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        ).also { it.gravity = Gravity.TOP })
        mv.visibility = View.INVISIBLE
        maneuverView = mv

        // ── Side buttons — RIGHT side, just below maneuver banner ─────────────
        // Position: top margin = maneuver banner height (~180dp) + 8dp gap
        val sideCol = LinearLayout(context).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER_HORIZONTAL
            visibility = View.INVISIBLE
        }
        root.addView(sideCol, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.WRAP_CONTENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        ).also {
            it.gravity = Gravity.TOP or Gravity.END
            it.setMargins(0, (188 * dp).toInt(), (12 * dp).toInt(), 0)
        })
        sideButtons = sideCol

        val btnSize = (56 * dp).toInt()
        val btnParams = LinearLayout.LayoutParams(btnSize, btnSize).also {
            it.bottomMargin = (10 * dp).toInt()
        }

        // Button 1: Mute/Unmute (speaker icon)
        val muteBtn = makeIconButton(drawSpeakerIcon(false)) { toggleMute() }
        sideCol.addView(muteBtn, btnParams)
        btnMuteView = muteBtn

        // Button 2: Overview / Route view
        val overviewBtn = makeIconButton(drawRouteOverviewIcon()) { toggleOverview() }
        sideCol.addView(overviewBtn, LinearLayout.LayoutParams(btnSize, btnSize).also {
            it.bottomMargin = (10 * dp).toInt()
        })
        btnOverviewView = overviewBtn

        // Button 3: Recenter / return to follow mode — ALWAYS visible, below button 2
        // Greyed out (lower opacity) when already following; full opacity in overview mode
        val recenterBtn = makeIconButton(drawNavigationArrowIcon()) { recenterCamera() }
        recenterBtn.alpha = 0.4f  // dimmed by default (already following)
        sideCol.addView(recenterBtn, LinearLayout.LayoutParams(btnSize, btnSize))
        btnRecenterView = recenterBtn

        // ── SpeedInfoView — bottom-left, above ETA bar ────────────────────────
        // FIX: use WRAP_CONTENT instead of fixed 72x72dp — the native Mapbox
        // component renders both the posted speed limit sign AND the current
        // vehicle speed side-by-side, which needs flexible width
        val siv = MapboxSpeedInfoView(context)
        // FIX: the ETA bar below has an explicit elevation (8dp) and is
        // added to the layout AFTER this view — in Android, both of those
        // independently favor the ETA bar drawing ON TOP when their bounds
        // overlap. speedInfoView had no elevation of its own (defaulting to
        // 0), so if the ETA bar's actual rendered height (WRAP_CONTENT,
        // varies with its text content) ever extends slightly further up
        // than the 88dp bottom margin below assumes, the speed limit panel
        // could end up fully or partially hidden behind it — even though
        // its visibility and rendered content are otherwise both correct.
        // Giving it a higher elevation guarantees it always draws on top,
        // regardless of any such overlap, without changing any position or
        // margin values (zero visual change in the non-overlapping case).
        siv.elevation = 12 * dp
        root.addView(siv as View, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.WRAP_CONTENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        ))
        siv.visibility = View.GONE
        speedInfoView = siv
        applySpeedLimitPosition()

        // ── ETA bottom bar ─────────────────────────────────────────────────────
        val resolvedEtaBg = etaBarBackgroundColor?.let { parseColorSafe(it) } ?: Color.parseColor("#1E2433")
        val resolvedEtaText = etaTextColor?.let { parseColorSafe(it) } ?: Color.WHITE
        val bar = LinearLayout(context).apply {
            orientation = LinearLayout.HORIZONTAL
            setBackgroundColor(resolvedEtaBg)
            elevation = 8 * dp
            visibility = View.INVISIBLE
            gravity = Gravity.CENTER_VERTICAL
            setPadding(
                (16 * dp).toInt(), (12 * dp).toInt(),
                (16 * dp).toInt(), (12 * dp).toInt()
            )
        }
        root.addView(bar, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        ).also { it.gravity = Gravity.BOTTOM })

        val etaTime = TextView(context).apply {
            textSize = 24f; setTextColor(resolvedEtaText)
            typeface = android.graphics.Typeface.DEFAULT_BOLD
        }
        bar.addView(etaTime, LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f))
        tvEtaTime = etaTime

        val center = LinearLayout(context).apply {
            orientation = LinearLayout.VERTICAL; gravity = Gravity.CENTER
        }
        val dur = TextView(context).apply {
            textSize = 18f; setTextColor(resolvedEtaText)
            typeface = android.graphics.Typeface.DEFAULT_BOLD; gravity = Gravity.CENTER
        }
        val dist = TextView(context).apply {
            textSize = 13f; setTextColor(Color.parseColor("#AAAAAA")); gravity = Gravity.CENTER
        }
        center.addView(dur); center.addView(dist)
        bar.addView(center, LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 2f))
        tvDuration = dur; tvDistance = dist


        val cancelBtn = TextView(context).apply {
            text = "✕"; textSize = 22f; setTextColor(Color.parseColor("#AAAAAA"))
            gravity = Gravity.END or Gravity.CENTER_VERTICAL
            setOnClickListener { cancelNavigation() }
        }
        bar.addView(cancelBtn, LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f))
        etaBar = bar

        // TripProgressView — 1×1 hidden (required for render() API)
        val tpv = MapboxTripProgressView(context)
        root.addView(tpv as View, FrameLayout.LayoutParams(1, 1))
        tpv.visibility = View.GONE
        tripProgressView = tpv

        // FIX: neither the ETA bar nor the speed limit panel previously
        // accounted for the system navigation bar (3-button or gesture) at
        // all — confirmed missing, and confirmed to matter: reported
        // working correctly in an emulator (whose default nav bar
        // configuration reserves little/no bottom space) but the ETA bar
        // not appearing on real hardware, which does reserve real space
        // there. Without this, bottom-anchored elements can render fully or
        // partially UNDER the system bar — genuinely marked visible in
        // code, but not visible on screen. This listener keeps
        // lastSystemBarInsets current and re-applies both elements'
        // position whenever insets change (rotation, nav bar
        // show/hide, etc.) — also relevant independent of any specific
        // device, since edge-to-edge display is becoming mandatory rather
        // than optional starting Android 15+.
        ViewCompat.setOnApplyWindowInsetsListener(root) { _, insets ->
            applySystemBarInsets(insets.getInsets(WindowInsetsCompat.Type.systemBars()))
            insets
        }
        ViewCompat.requestApplyInsets(root)

        // FIX: seed lastSystemBarInsets immediately with a direct read from
        // the Activity's decorView (see fetchSystemBarInsetsDirectly()'s
        // full reasoning above, near lastSystemBarInsets' declaration),
        // rather than relying solely on the listener above ever firing.
        // On Android 16 (edge-to-edge enforced, no opt-out possible) with
        // react-native-safe-area-context in the host app, that dispatch
        // can be consumed higher up the RN view tree before ever reaching
        // this nested native view — which previously left
        // lastSystemBarInsets stuck at Insets.NONE for this view's entire
        // lifetime, and the ETA bar positioned under the system navigation
        // bar as a result. A harmless, idempotent no-op wherever the
        // listener above already fires correctly (applySystemBarInsets
        // would simply be called twice with the same value).
        applySystemBarInsets(fetchSystemBarInsetsDirectly())

        addView(root)
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Init APIs
    // ─────────────────────────────────────────────────────────────────────────
    private fun initAPIs() {
        val distanceFormatterOptions = DistanceFormatterOptions.Builder(context).build()

        maneuverApi = MapboxManeuverApi(MapboxDistanceFormatter(distanceFormatterOptions))

        tripProgressApi = MapboxTripProgressApi(
            TripProgressUpdateFormatter.Builder(context)
                .distanceRemainingFormatter(DistanceRemainingFormatter(distanceFormatterOptions))
                .timeRemainingFormatter(TimeRemainingFormatter(context))
                .estimatedTimeToArrivalFormatter(
                    EstimatedTimeToArrivalFormatter(context, TimeFormat.NONE_SPECIFIED)
                )
                .build()
        )

        speedInfoApi = MapboxSpeedInfoApi()

        routeLineApi = MapboxRouteLineApi(MapboxRouteLineApiOptions.Builder().build())
        routeLineView = MapboxRouteLineView(
            MapboxRouteLineViewOptions.Builder(context)
                .routeLineBelowLayerId("road-label-navigation")
                .build()
        )

        routeArrowView = MapboxRouteArrowView(RouteArrowOptions.Builder(context).build())

        // ── Voice APIs ─────────────────────────────────────────────────────────
        // Use device locale for TTS, fall back to "en" if needed
        val voiceLang = language ?: Locale.getDefault().language.let {
            if (it.isEmpty()) "en" else it
        }
        speechApi = MapboxSpeechApi(context, voiceLang)
        voiceInstructionsPlayer = MapboxVoiceInstructionsPlayer(context, voiceLang)
    }

    // ── Voice callbacks — EXACT pattern from official TurnByTurnExperienceActivity ──
    // MapboxNavigationConsumer is in com.mapbox.navigation.ui.base.util (NOT base.route)
    // voiceInstructionsPlayerCallback must be defined BEFORE speechCallback (used inside it)
    private val voiceInstructionsPlayerCallback =
        com.mapbox.navigation.ui.base.util.MapboxNavigationConsumer<SpeechAnnouncement> { value ->
            speechApi.clean(value)
        }

    private val speechCallback =
        com.mapbox.navigation.ui.base.util.MapboxNavigationConsumer<
            com.mapbox.bindgen.Expected<SpeechError, SpeechValue>
        > { expected ->
            expected.fold(
                { error ->
                    if (!isMuted) {
                        voiceInstructionsPlayer.play(error.fallback, voiceInstructionsPlayerCallback)
                    }
                },
                { value ->
                    if (!isMuted) {
                        voiceInstructionsPlayer.play(value.announcement, voiceInstructionsPlayerCallback)
                    }
                }
            )
        }

    // OBSERVER TEARDOWN FIX (5.0.10): these three were previously anonymous
    // objects created INLINE inside registerObservers() - impossible to
    // unregister later since nothing held a reference to the exact same
    // instance. Named class-level properties here (bodies UNCHANGED,
    // verbatim from before - only where the object is created moved, not
    // what it does) so onDetachedFromWindow() can unregister them
    // symmetrically, alongside voiceInstructionsObserver below (which
    // already had this pattern - only IT was ever unregistered on
    // detach). Real device crash this fixes: "This instance of
    // MapboxNavigation is destroyed" thrown from deep inside a
    // Choreographer/Fabric frame callback (not from any of this file's own
    // onRoutesReady/onCanceled/onFailure call sites, all already guarded) -
    // consistent with ONE of these three still-registered observers
    // firing AFTER MapboxNavigationProvider.destroy() tore the instance
    // down, since none of them were ever unregistered before. Safe to
    // construct this early (class-property initializers, not referenced
    // until registerObservers() runs from setupNavigation()'s ASYNC
    // loadStyle callback - by which point construction has long finished),
    // same reasoning already relied on elsewhere in this file (e.g.
    // fetchHandler).
    private val routesObserver = object : RoutesObserver {
        override fun onRoutesChanged(result: RoutesUpdatedResult) {
            if (result.navigationRoutes.isNotEmpty()) {
                // Cancel any in-flight speech when route changes
                speechApi.cancel()
                voiceInstructionsPlayer.clear()

                routeLineApi.setNavigationRoutes(result.navigationRoutes) { value ->
                    mapView.mapboxMap.style?.apply { routeLineView.renderRouteDrawData(this, value) }
                }
                viewportDataSource.onRouteChanged(result.navigationRoutes.first())
                viewportDataSource.evaluate()
                isOverviewMode = false
                safeCameraFollowing()
                showUI()
                onRoutesReady(mapOf(
                    "routeCount" to result.navigationRoutes.size,
                    "distanceMeters" to (result.navigationRoutes.first().directionsRoute.distance() ?: 0.0),
                    "durationSeconds" to (result.navigationRoutes.first().directionsRoute.duration() ?: 0.0)
                ))
            } else {
                mapView.mapboxMap.style?.let { style ->
                    routeLineApi.clearRouteLine { value ->
                        routeLineView.renderClearRouteLineValue(style, value)
                    }
                    routeArrowView.render(style, routeArrowApi.clearArrows())
                }
                viewportDataSource.clearRouteData()
                viewportDataSource.evaluate()
                hideUI()
            }
        }
    }

    private val routeProgressObserver = RouteProgressObserver { routeProgress ->
        viewportDataSource.onRouteProgressChanged(routeProgress)
        viewportDataSource.evaluate()

        mapView.mapboxMap.style?.let { style ->
            val arrowResult = routeArrowApi.addUpcomingManeuverArrow(routeProgress)
            routeArrowView.renderManeuverUpdate(style, arrowResult)
        }

        val maneuvers = maneuverApi.getManeuvers(routeProgress)
        maneuvers.fold(
            { error -> Log.w(TAG, "Maneuver error: ${error.errorMessage}"); Unit },
            { _ ->
                maneuverView?.visibility = View.VISIBLE
                maneuverView?.renderManeuvers(maneuvers)
                Unit
            }
        )

        val tripProgress = tripProgressApi.getTripProgress(routeProgress)
        tripProgressView?.render(tripProgress)
        updateEtaBar(
            tripProgress.estimatedTimeToArrival,
            tripProgress.totalTimeRemaining,
            tripProgress.distanceRemaining
        )

        onRouteProgressChanged(mapOf(
            "distanceRemaining" to routeProgress.distanceRemaining,
            "durationRemaining" to routeProgress.durationRemaining,
            "distanceTraveled" to routeProgress.distanceTraveled,
            "fractionTraveled" to routeProgress.fractionTraveled,
            "currentStepDistanceRemaining" to
                (routeProgress.currentLegProgress?.currentStepProgress?.distanceRemaining ?: 0f)
        ))
    }

    private val locationObserver = object : LocationObserver {
        override fun onNewRawLocation(rawLocation: Location) {}
        override fun onNewLocationMatcherResult(result: LocationMatcherResult) {
            val loc = result.enhancedLocation
            // ── FIX: Puck jitter / drift ──────────────────────────────────────
            // Root cause (confirmed via Mapbox GitHub issue #4140):
            // When keyPoints are passed to changePosition(), the puck animator
            // splits the 1-second transition EVENLY across all keypoints in time,
            // but the keypoints are NOT evenly spaced in distance. This causes the
            // puck to speed up/slow down and visibly drift left/right off the
            // route line, even on a perfectly straight road.
            // Official Mapbox workaround: pass emptyList() instead of keyPoints.
            navigationLocationProvider.changePosition(location = loc, keyPoints = emptyList())
            viewportDataSource.onLocationChanged(loc)
            viewportDataSource.evaluate()

            if (!firstLocationReceived) {
                firstLocationReceived = true
                safeCameraFollowing()
            }

            // ── Speed limit ───────────────────────────────────────────────────
            // FIX (Android-vs-iOS parity audit): updatePostedAndCurrentSpeed()
            // requires BOTH the posted speed limit AND the GPS-derived current
            // speed (result.enhancedLocation.speed, nullable — confirmed via
            // MapboxSpeedInfoApi/SpeedLimitProcessor source at v3.20.x) to be
            // non-null — it returns null, discarding a perfectly valid posted
            // limit, whenever current speed alone is momentarily/persistently
            // unavailable (common right after guidance starts, weak GPS,
            // certain location providers). iOS's own SpeedLimitView.canDraw
            // (verified verbatim at v3.20.1) has no such dependency — it shows
            // the sign whenever a posted limit is known, full stop, which is
            // why iOS reliably shows speed limits and Android didn't. Both
            // platforms read the same underlying native `speedLimit` field
            // (confirmed: iOS's MapboxNavigator.swift and Android's
            // LocationMatcherResult both source it from the shared native
            // Navigator), so this is a platform code-path gap, not a data
            // coverage difference between iOS and Android.
            val fmtOptions = DistanceFormatterOptions.Builder(context).build()
            val speedInfo = speedInfoApi.updatePostedAndCurrentSpeed(result, fmtOptions)
            if (speedInfo != null) {
                speedInfoView?.visibility = View.VISIBLE
                speedInfoView?.render(speedInfo)
            } else {
                // Posted limit may still be known even though the SDK's own
                // helper discarded it for lack of current speed — render it
                // ourselves (posted-only, matching iOS) instead of hiding the
                // whole widget. Falls back to fully hidden only when the
                // posted limit itself is genuinely unavailable too — same
                // end condition as before this fix.
                renderPostedSpeedOnly(result.speedLimitInfo, fmtOptions.unitType)
            }
            logSpeedLimitState(
                result,
                when {
                    speedInfo != null -> "sdk-render"
                    result.speedLimitInfo.speed != null -> "posted-only-fallback"
                    else -> "hidden"
                },
            )

            checkAndSwitchDayNight()
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Renders the posted speed limit only, bypassing MapboxSpeedInfoApi /
    // SpeedInfoValue entirely (used when the SDK's own updatePostedAndCurrentSpeed()
    // returned null solely because current speed is unknown — see the FIX
    // comment above in onNewLocationMatcherResult).
    //
    // SpeedInfoValue and LocationMatcherResult both have an `internal
    // constructor` (verified at v3.20.x source) — this package, being a
    // downstream consumer module, cannot construct either, so we can't hand
    // MapboxSpeedInfoView.render() a currentSpeed-less SpeedInfoValue built
    // ourselves. Instead we drive MapboxSpeedInfoView's own PUBLIC child
    // views directly (speedInfoMutcdLayout, speedInfoPostedSpeedMutcd, etc.
    // — all public `val`s on the class, verified at v3.20.x source), which
    // is the only way to reach this state through supported public API.
    // ─────────────────────────────────────────────────────────────────────────
    private fun renderPostedSpeedOnly(info: SpeedLimitInfo, unitType: UnitType) {
        val siv = speedInfoView
        val postedRaw = info.speed
        if (siv == null || postedRaw == null) {
            speedInfoView?.visibility = View.GONE
            return
        }
        siv.visibility = View.VISIBLE
        val postedSpeed = convertPostedSpeed(postedRaw.toDouble(), info.unit, unitType)
        when (unitType) {
            UnitType.IMPERIAL -> siv.speedInfoUnitTextMutcd.text = "mph"
            UnitType.METRIC -> siv.speedInfoUnitTextMutcd.text = "km/h"
        }
        // Current-speed sub-views are always hidden here — we have no
        // current speed to show (that's the whole reason this path runs) —
        // matching how MapboxSpeedInfoView.render() itself hides them
        // whenever posted-only data is rendered.
        when (info.sign) {
            SpeedLimitSign.VIENNA -> {
                siv.speedInfoViennaLayout.visibility = View.VISIBLE
                siv.speedInfoMutcdLayout.visibility = View.GONE
                siv.speedInfoPostedSpeedVienna.text = postedSpeed.toString()
                siv.speedInfoCurrentSpeedVienna.visibility = View.GONE
            }
            else -> { // MUTCD — also MapboxSpeedInfoView's own default when sign is unrecognized
                siv.speedInfoMutcdLayout.visibility = View.VISIBLE
                siv.speedInfoViennaLayout.visibility = View.GONE
                siv.speedInfoPostedSpeedMutcd.text = postedSpeed.toString()
                siv.speedInfoCurrentSpeedMutcd.visibility = View.GONE
            }
        }
    }

    // Verbatim port of the SDK's own (private, hence unreachable from this
    // module) PostedAndCurrentSpeedFormatter.getPostedSpeedLimit() —
    // SpeedLimitInfo.speed is posted-limit-only, never meters/second (that
    // unit only ever applies to current speed), so only the KM/H and MPH
    // source branches are reachable in practice; METERS_PER_SECOND is kept
    // for exhaustiveness since SpeedUnit is a `when` subject. Kept numerically
    // identical to the SDK's own formula so the displayed number never
    // differs from what the normal (currentSpeed-available) render path
    // would show for the same road.
    private fun convertPostedSpeed(speed: Double, fromUnit: SpeedUnit, toUnit: UnitType): Int {
        return when (fromUnit) {
            SpeedUnit.KILOMETERS_PER_HOUR -> when (toUnit) {
                UnitType.IMPERIAL -> 5 * ((speed / 1.609) / 5).roundToInt()
                UnitType.METRIC -> speed.roundToInt()
            }
            SpeedUnit.MILES_PER_HOUR -> when (toUnit) {
                UnitType.IMPERIAL -> speed.roundToInt()
                UnitType.METRIC -> 5 * (speed * 1.609 / 5).roundToInt()
            }
            SpeedUnit.METERS_PER_SECOND -> when (toUnit) {
                UnitType.IMPERIAL -> (speed / 1000.0 / 1.609 * 3600).roundToInt()
                UnitType.METRIC -> (speed / 1000.0 * 3600).roundToInt()
            }
        }
    }

    // Diagnostic (5.1.2): logs the exact speed-limit data state so
    // `adb logcat -s ExpoMapboxNavigation` on a real device can prove
    // whether the posted limit is absent from the DATA for the current
    // road segment (a coverage limitation — nothing any code can render)
    // or present but not rendered (which would be a real code bug).
    // Logs only on state TRANSITIONS (posted value / current-speed
    // availability / rendered path changes), not every location tick,
    // so a whole drive produces a handful of lines, not thousands.
    private var lastSpeedLogKey: String? = null
    private fun logSpeedLimitState(result: LocationMatcherResult, renderedPath: String) {
        val info = result.speedLimitInfo
        val currentSpeedAvailable = result.enhancedLocation.speed != null
        val key = "${info.speed}|${info.unit}|${info.sign}|$currentSpeedAvailable|$renderedPath"
        if (key != lastSpeedLogKey) {
            lastSpeedLogKey = key
            Log.d(
                TAG,
                "SpeedLimit state: posted=${info.speed} unit=${info.unit} sign=${info.sign} " +
                    "currentSpeedAvailable=$currentSpeedAvailable rendered=$renderedPath",
            )
        }
        // Follow-up diagnostic: data/render-path alone doesn't prove pixels
        // actually appeared on screen — a confirmed real-device test showed
        // posted=50/20, rendered=sdk-render, yet nothing visible. Posted to
        // the view's own message queue (not read synchronously here) so it
        // runs AFTER the layout pass this frame's visibility/text changes
        // trigger — reading geometry synchronously here would still show
        // stale (pre-layout) values.
        speedInfoView?.post {
            val siv = speedInfoView ?: return@post
            val loc = IntArray(2)
            siv.getLocationOnScreen(loc)
            val dm = context.resources.displayMetrics
            Log.d(
                TAG,
                "SpeedLimit geometry: visibility=${siv.visibility} isShown=${siv.isShown} " +
                    "w=${siv.width} h=${siv.height} screenX=${loc[0]} screenY=${loc[1]} " +
                    "screenW=${dm.widthPixels} screenH=${dm.heightPixels} dp=$dp " +
                    "topInset=${lastSystemBarInsets.top} bottomInset=${lastSystemBarInsets.bottom} " +
                    "position=$speedLimitPosition alpha=${siv.alpha} " +
                    "parentIsRoot=${siv.parent != null}",
            )
        }
    }

    private val voiceInstructionsObserver = VoiceInstructionsObserver { voiceInstructions ->
        speechApi.generate(voiceInstructions, speechCallback)
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Setup Navigation
    // ─────────────────────────────────────────────────────────────────────────
    private fun setupNavigation() {
        mapboxNavigation = MapboxNavigationProvider.create(
            NavigationOptions.Builder(context).build()
        )

        // STYLE-LOAD FAILURE RECOVERY (5.0.9): loadStyle(...)'s trailing
        // lambda below is success-ONLY - Mapbox v11 dropped the old
        // onMapLoadErrorListener parameter with no direct replacement wired
        // to loadStyle itself (confirmed against Mapbox's own v11 API:
        // error detection moved to a SEPARATE subscription,
        // MapboxMap.subscribeMapLoadingError). Without this, a style load
        // that genuinely fails (transient network gap at cold start, style
        // host unreachable, etc. - NOT the fast-device race the 5.0.3
        // readiness-ordering fix already handles) left
        // navigationSetupComplete permanently false: real device report -
        // map stuck at world-view zoom forever, no route drawn, no camera
        // engagement, and total silence to JS (fetchRoutes() just kept
        // deferring via pendingRouteFetchUntilSetup, no onRoutesFailed
        // ever fired). The 5.0.3 fix's whole design assumes loadStyle's
        // callback eventually fires - true for a race, false for an
        // outright failure. This subscription is the missing failure path.
        //
        // Bounded, not self-perpetuating: subscribeMapLoadingError can fire
        // more than once for a single underlying failure (per sub-resource
        // - style, sprite, glyphs, tiles), so the retry count is a capped
        // counter (MAX_STYLE_LOAD_RETRIES), never reset by further error
        // events. Guarded by navigationSetupComplete so a late/spurious
        // error firing AFTER a successful load can never trigger a wasted
        // reload of an already-working map.
        mapView.mapboxMap.subscribeMapLoadingError { eventData ->
            if (navigationSetupComplete) return@subscribeMapLoadingError
            Log.e(TAG, "Map style load error (type=${eventData.type}): ${eventData.message}")
            if (styleLoadRetryCount < MAX_STYLE_LOAD_RETRIES) {
                styleLoadRetryCount++
                Log.w(TAG, "Retrying map style load (attempt $styleLoadRetryCount/$MAX_STYLE_LOAD_RETRIES)")
                fetchHandler.postDelayed({ attemptLoadStyle() }, STYLE_LOAD_RETRY_DELAY_MS)
            } else {
                Log.e(TAG, "Map style load failed after $MAX_STYLE_LOAD_RETRIES retries - giving up")
                onRoutesFailed(mapOf("message" to "Map failed to load: ${eventData.message}"))
            }
        }

        attemptLoadStyle()
    }

    // Extracted from setupNavigation() (5.0.9) so subscribeMapLoadingError's
    // retry path can re-invoke the exact same style-load logic without
    // duplicating it. The success callback body is otherwise UNCHANGED from
    // 5.0.3 - only the retry entry point is new.
    private fun attemptLoadStyle() {
        mapView.mapboxMap.loadStyle(mapStyle ?: getAutoStyle()) { style ->
            routeLineView.initializeLayers(style)
            applyCustomRasterLayer(style)

            viewportDataSource = MapboxNavigationViewportDataSource(mapView.mapboxMap)
            viewportDataSource.followingPadding = followingPadding
            viewportDataSource.overviewPadding = overviewPadding

            // Waze-style tilted 3D following view — also helps stabilize the
            // visual framing since pitch reduces perceived jitter from GPS noise
            viewportDataSource.options.followingFrameOptions.defaultPitch = 45.0
            viewportDataSource.options.followingFrameOptions.maxZoom = 17.0

            navigationCamera = NavigationCamera(
                mapView.mapboxMap,
                mapView.camera,
                viewportDataSource
            )

            // Navigation arrow puck
            mapView.location.setLocationProvider(navigationLocationProvider)
            applyPuckSettings()
            // FIXED PUCK ORIENTATION (5.1.0): was `PuckBearing.COURSE` -
            // real device video evidence showed the puck visibly rotating
            // WHILE the road curved on screen underneath it, instead of
            // staying fixed pointing up like Waze/Google Maps.
            //
            // Root cause, confirmed at the Mapbox Maps SDK source: the
            // location-indicator layer's `bearing` is a GEOGRAPHIC property
            // (relative to true north), rendered through the SAME camera
            // transform as the rest of the map (verified verbatim in
            // mapbox-maps-android's LocationIndicatorLayerRenderer.kt -
            // `layer.bearing(bearing)` sets the style layer's bearing
            // directly). `PuckBearing.COURSE` fed the puck's RAW,
            // instantaneous GPS course on every location update. The
            // camera's own bearing, meanwhile, comes from
            // MapboxNavigationViewportDataSource's `bearingSmoothing`
            // (`getSmootherBearingForMap`, capped at 45deg per update,
            // computed from an anticipatory look-ahead window of upcoming
            // route points - by design, NOT raw instantaneous course, for a
            // less jittery camera). These two bearings are computed by
            // different algorithms with different responsiveness, so they
            // drift apart mid-turn - and since the puck's on-screen angle
            // is literally (puck bearing - camera bearing), any drift
            // between them is exactly what appears as "the puck rotating".
            //
            // Fix: `PuckBearing.HEADING` - which is also the Mapbox Maps
            // SDK's own DEFAULT (`@Default("PuckBearing.HEADING")`,
            // verified verbatim in LocationComponentSettingsData.kt) and
            // exactly what Mapbox's own official reference implementation
            // uses (TurnByTurnExperienceActivity.kt, mapbox-navigation-
            // android-examples repo - never overrides puckBearing at all,
            // relying on this same default). HEADING mode drives puck
            // rotation from device COMPASS data, not the `Location.bearing`
            // fed via `navigationLocationProvider.changePosition(...)` -
            // since this package never feeds compass data through that
            // pathway, the puck's on-screen bearing simply never gets
            // pushed away from the camera's own bearing by a competing
            // update source. The net effect is exactly the Waze pattern:
            // 100% of the visible rotation comes from the camera turning
            // beneath a puck that stays visually fixed - not from a manual
            // bearing-cancellation calculation, but because there is no
            // second, independently-timed bearing source left to disagree
            // with the camera in the first place.
            //
            // REFINEMENT: HEADING alone has one edge case, found during
            // audit - Overview mode (this file's own toggleOverview(),
            // camera goes north-up/non-rotating). There, HEADING gives no
            // direction at all (no compass feed), whereas the OLD COURSE
            // mode showed the correct absolute direction just fine (a
            // static, non-rotating map has no camera-bearing-vs-course
            // drift to begin with - COURSE was never actually buggy there).
            // So: switch puckBearing WITH the camera state instead of
            // fixing it to one mode - HEADING only while Following (the
            // mode that was actually broken), COURSE everywhere else
            // (Overview/Idle, where it was always correct). Registered
            // once, right after NavigationCamera's construction below -
            // registerNavigationCameraStateChangeObserver invokes the
            // callback immediately with the CURRENT state upon
            // registration (verified verbatim in NavigationCamera.kt), so
            // this also sets the correct initial value with no separate
            // explicit assignment needed.
            navigationCamera.registerNavigationCameraStateChangeObserver { cameraState ->
                mapView.location.puckBearing = when (cameraState) {
                    NavigationCameraState.TRANSITION_TO_FOLLOWING,
                    NavigationCameraState.FOLLOWING -> PuckBearing.HEADING
                    NavigationCameraState.TRANSITION_TO_OVERVIEW,
                    NavigationCameraState.OVERVIEW,
                    NavigationCameraState.IDLE -> PuckBearing.COURSE
                }
            }

            registerObservers()
            // READINESS ORDERING FIX (5.0.3): the completion flag is set
            // LAST, only after the route line layers, viewport data source,
            // navigation camera, location provider AND all observers are in
            // place. It was previously set before registerObservers(),
            // which left a window where fetchRoutes() considered the view
            // ready while route/location observers weren't registered yet.
            // This whole loadStyle callback is ASYNC (style loading is
            // network-bound): on fast devices the route response can win
            // the race against it - real reports on recent Samsung/Xiaomi
            // hardware showed routes arriving before setup completed, with
            // the map stuck on a world view and the camera never engaging.
            navigationSetupComplete = true
            // Replay a fetch that was deferred by fetchRoutes' readiness
            // guard while setup was still in progress (see
            // pendingRouteFetchUntilSetup) - the coalescing scheduler
            // makes this safe to call unconditionally-guarded.
            if (pendingRouteFetchUntilSetup) {
                pendingRouteFetchUntilSetup = false
                scheduleFetchRoutes()
            }
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Observers
    // ─────────────────────────────────────────────────────────────────────────
    private fun registerObservers() {
        val nav = mapboxNavigation ?: return
        // Bodies moved to named class-level properties above (5.0.10) so
        // onDetachedFromWindow() can unregister every one of these
        // symmetrically - see the comment on routesObserver above for why.
        nav.registerRoutesObserver(routesObserver)
        nav.registerRouteProgressObserver(routeProgressObserver)
        nav.registerLocationObserver(locationObserver)
        nav.registerVoiceInstructionsObserver(voiceInstructionsObserver)
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Button actions
    // ─────────────────────────────────────────────────────────────────────────
    private fun toggleMute() {
        isMuted = !isMuted
        // Update icon to show muted/unmuted state
        btnMuteView?.setImageBitmap(drawSpeakerIcon(isMuted))
        // Apply volume via SpeechVolume API
        voiceInstructionsPlayer.volume(SpeechVolume(if (isMuted) 0f else 1f))
    }

    private fun toggleOverview() {
        if (isOverviewMode) {
            recenterCamera()
        } else {
            isOverviewMode = true
            // Button 3 lights up (full opacity) to invite the user to tap it
            btnRecenterView?.alpha = 1f
            try {
                navigationCamera.requestNavigationCameraToOverview()
            } catch (e: Exception) {
                Log.e(TAG, "Camera overview error: ${e.message}")
            }
        }
    }

    private fun recenterCamera() {
        isOverviewMode = false
        // Button 3 dims again — already in following mode
        btnRecenterView?.alpha = 0.4f
        safeCameraFollowing()
    }

    // ─────────────────────────────────────────────────────────────────────────
    // ETA bar
    // ─────────────────────────────────────────────────────────────────────────
    private fun updateEtaBar(
        estimatedTimeToArrivalMs: Long,
        totalTimeRemainingSec: Double,
        distanceRemainingMetres: Double
    ) {
        val arrivalCal = Calendar.getInstance().apply { timeInMillis = estimatedTimeToArrivalMs }
        tvEtaTime?.text = String.format(
            "%02d:%02d", arrivalCal.get(Calendar.HOUR_OF_DAY), arrivalCal.get(Calendar.MINUTE)
        )
        val totalMin = (totalTimeRemainingSec / 60).toInt()
        tvDuration?.text = if (totalMin >= 60) "${totalMin / 60}h ${totalMin % 60}min" else "$totalMin min"
        tvDistance?.text = if (distanceRemainingMetres >= 1000)
            String.format("%.1f km", distanceRemainingMetres / 1000.0)
        else "${distanceRemainingMetres.toInt()} m"
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Day / Night
    // ─────────────────────────────────────────────────────────────────────────
    private fun getAutoStyle(): String {
        val hour = Calendar.getInstance().get(Calendar.HOUR_OF_DAY)
        return if (hour in 6..20) NavigationStyles.NAVIGATION_DAY_STYLE
        else NavigationStyles.NAVIGATION_NIGHT_STYLE
    }

    private fun checkAndSwitchDayNight() {
        val hour = Calendar.getInstance().get(Calendar.HOUR_OF_DAY)
        val shouldBeNight = hour !in 6..20
        if (shouldBeNight == isNightMode) return
        isNightMode = shouldBeNight
        // REVERTED (confirmed regression, reported directly): this used to
        // also call rebuildManeuverView() here, to switch the banner over to
        // maneuverBackgroundColorNight when night mode kicks in. That
        // rebuild-on-day/night-switch behavior did not exist before 3.0.0 —
        // before that release, this function only ever reloaded the map
        // style and never touched the maneuver view at all. Confirmed
        // (directly reported, comparing against 2.3.9's known-good behavior)
        // that adding it caused the banner to stop displaying ENTIRELY
        // during night mode — a regression, and a worse outcome than the
        // original problem (the banner just not changing color). Removed
        // outright rather than attempting another unconfirmed guess at
        // fixing the interaction — restoring known-stable pre-3.0.0 behavior
        // takes priority over the not-yet-working maneuverBackgroundColorNight
        // feature. The banner now keeps whatever background it already had
        // through a day/night transition, exactly like 2.3.9. Setting
        // maneuverBackgroundColorNight explicitly via its own prop setter is
        // unaffected by this revert — only the AUTOMATIC switch tied to the
        // time-based day/night check was removed.
        mapView.mapboxMap.loadStyle(
            if (shouldBeNight) NavigationStyles.NAVIGATION_NIGHT_STYLE else NavigationStyles.NAVIGATION_DAY_STYLE
        ) { style ->
            routeLineView.initializeLayers(style)
            applyCustomRasterLayer(style)
            mapboxNavigation?.getNavigationRoutes()?.takeIf { it.isNotEmpty() }?.let { routes ->
                routeLineApi.setNavigationRoutes(routes) { value ->
                    routeLineView.renderRouteDrawData(style, value)
                }
            }
        }
    }

    private fun safeCameraFollowing() {
        try {
            navigationCamera.requestNavigationCameraToFollowing(
                stateTransitionOptions = NavigationCameraTransitionOptions.Builder().maxDuration(0).build()
            )
        } catch (e: Exception) {
            Log.e(TAG, "Camera following error: ${e.message}")
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Feature: full route steps list (triggered by tapping the instruction
    // banner). Extracts every LegStep across all legs of the active route,
    // including lane guidance data when present on that step's BannerInstructions.
    // ─────────────────────────────────────────────────────────────────────────
    private fun emitFullRouteSteps() {
        val routes = mapboxNavigation?.getNavigationRoutes()
        val activeRoute = routes?.firstOrNull() ?: run {
            onManeuverBannerPressed(mapOf("steps" to emptyList<Map<String, Any>>()))
            return
        }

        val stepsPayload = mutableListOf<Map<String, Any>>()

        activeRoute.directionsRoute.legs()?.forEach { leg ->
            leg.steps()?.forEach { step ->
                val maneuver = step.maneuver()
                val firstBanner = step.bannerInstructions()?.firstOrNull()

                // Lane guidance: present on BannerInstructions.sub() when type == "lane"
                val laneData = firstBanner?.sub()?.components()
                    ?.filter { it.type() == "lane" }
                    ?.map { component ->
                        mapOf(
                            "active" to (component.active() ?: false),
                            "directions" to (component.directions() ?: emptyList<String>())
                        )
                    } ?: emptyList()

                stepsPayload.add(
                    mapOf(
                        "instruction" to (maneuver?.instruction() ?: ""),
                        "distanceMeters" to (step.distance() ?: 0.0),
                        "durationSeconds" to (step.duration() ?: 0.0),
                        "maneuverType" to (maneuver?.type() ?: ""),
                        "maneuverModifier" to (maneuver?.modifier() ?: ""),
                        "roadName" to (step.name() ?: ""),
                        "laneInstructions" to laneData
                    )
                )
            }
        }

        onManeuverBannerPressed(mapOf("steps" to stepsPayload))
    }

    // ── Native upcoming-steps panel (5.0.1) ─────────────────────────────────
    // Parity with iOS: tapping the banner there opens the SDK's own built-in
    // list of upcoming steps. Android's custom UI had no native equivalent
    // (the tap only emitted the event). This panel closes that gap, while
    // onManeuverBannerPressed keeps firing on every tap exactly as before.
    // Built entirely from framework widgets already used in this file - no
    // new dependency. All text shown comes from the Directions API response
    // (already localized via the `language` prop); the distance strings are
    // numeric - this package still hardcodes no user-facing prose.
    // Height is hard-capped at STEPS_PANEL_MAX_HEIGHT_FRACTION (86%) of the
    // view. (The iOS SDK list cannot be safely capped the same way: its
    // pin-to-screen-bottom constraint is internal and required-priority -
    // documented in the changelog.)
    private var rootLayout: FrameLayout? = null
    private var stepsPanelOverlay: FrameLayout? = null

    // ScrollView with a hard maximum height: standard onMeasure clamp. The
    // panel wraps its content when the list is short, scrolls inside the
    // cap when it is long.
    private inner class MaxHeightScrollView(
        context: Context,
        private val maxHeightPx: Int
    ) : android.widget.ScrollView(context) {
        override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
            val capped = MeasureSpec.makeMeasureSpec(maxHeightPx, MeasureSpec.AT_MOST)
            super.onMeasure(widthMeasureSpec, capped)
        }
    }

    // Numeric-only distance label (no translated words, so nothing for this
    // package to localize) - unit system follows the voiceUnits/language
    // props via resolveVoiceUnits(), same as the route request itself.
    private fun formatStepDistance(meters: Double): String {
        return if (resolveVoiceUnits() == "imperial") {
            val feet = meters * 3.28084
            if (feet < 1000) "${feet.toInt()} ft"
            else String.format(java.util.Locale.getDefault(), "%.1f mi", meters / 1609.344)
        } else {
            if (meters < 1000) "${meters.toInt()} m"
            else String.format(java.util.Locale.getDefault(), "%.1f km", meters / 1000.0)
        }
    }

    private fun dismissNativeStepsPanel() {
        stepsPanelOverlay?.let { rootLayout?.removeView(it) }
        stepsPanelOverlay = null
    }

    // ── Waypoint markers (5.0.2) ────────────────────────────────────────────
    // A checkered destination flag at the final coordinate and numbered
    // circular badges at intermediate leg-separating waypoints - the same
    // markers the iOS side renders through the SDK's delegate hooks. Android
    // uses the Maps SDK's annotation plugin (bundled with the existing
    // com.mapbox.maps:android dependency - no new artifact) with bitmaps
    // drawn in code, matching this file's existing Canvas-drawn-icon
    // convention (see drawSpeakerIcon and friends). Markers are added when
    // routes become active and fully cleared on navigation teardown.
    private var waypointAnnotationManager: PointAnnotationManager? = null

    private fun drawDestinationFlagBitmap(): Bitmap {
        // GEOMETRY CONTRACT (5.0.3 offset fix): the pole's base sits EXACTLY
        // at the bitmap's bottom-center pixel, because IconAnchor.BOTTOM
        // anchors the bitmap's bottom-center on the map point. The previous
        // drawing had the pole at the LEFT edge with a 2dp empty margin
        // below it - so the flag rendered visibly offset from the true
        // point (confirmed by a real device screenshot). Pole is now
        // horizontally centered and touches the bottom edge; the flag
        // banner waves to the right of the pole top.
        //
        // SIZE (5.1.0, "more prominent" per real-device feedback the flag
        // was too small next to the route line): scaled up from the
        // previous 36x40dp to 48x56dp - same proportions/design (pole +
        // 3x2 checkered banner) applied identically on iOS in this same
        // version, so both platforms render the same design at the same
        // relative size.
        val w = (48 * dp).toInt()
        val h = (56 * dp).toInt()
        val bmp = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(bmp)
        val darkSlate = Color.rgb(46, 59, 74)
        val ink = Color.rgb(31, 36, 43)
        val paint = Paint(Paint.ANTI_ALIAS_FLAG)
        val poleCenterX = w / 2f
        // Pole: rounded vertical bar, horizontally centered, base flush
        // with the bitmap's bottom edge (the anchor point).
        paint.color = darkSlate
        canvas.drawRoundRect(
            RectF(poleCenterX - 2f * dp, 3 * dp, poleCenterX + 2f * dp, h.toFloat()),
            2f * dp, 2f * dp, paint
        )
        // Flag body: white base, 3x2 checker, thin border - attached to the
        // pole top, extending right.
        val flag = RectF(poleCenterX + 2f * dp, 4 * dp, poleCenterX + 22f * dp, 22 * dp)
        paint.color = Color.WHITE
        canvas.drawRect(flag, paint)
        paint.color = ink
        val cellW = flag.width() / 3f
        val cellH = flag.height() / 2f
        for (row in 0 until 2) {
            for (col in 0 until 3) {
                if ((row + col) % 2 == 0) {
                    canvas.drawRect(
                        flag.left + col * cellW,
                        flag.top + row * cellH,
                        flag.left + (col + 1) * cellW,
                        flag.top + (row + 1) * cellH,
                        paint
                    )
                }
            }
        }
        paint.style = Paint.Style.STROKE
        paint.strokeWidth = 1 * dp
        canvas.drawRect(flag, paint)
        return bmp
    }

    private fun drawWaypointBadgeBitmap(number: Int): Bitmap {
        val size = (26 * dp).toInt()
        val bmp = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(bmp)
        val cx = size / 2f
        val paint = Paint(Paint.ANTI_ALIAS_FLAG)
        // Disc: dark slate fill + white stroke (same palette as the flag).
        paint.color = Color.rgb(46, 59, 74)
        canvas.drawCircle(cx, cx, cx - 1.5f * dp, paint)
        paint.style = Paint.Style.STROKE
        paint.strokeWidth = 2 * dp
        paint.color = Color.WHITE
        canvas.drawCircle(cx, cx, cx - 1.5f * dp, paint)
        // Number, white, centered.
        paint.style = Paint.Style.FILL
        paint.textSize = 12 * dp
        paint.textAlign = Paint.Align.CENTER
        paint.isFakeBoldText = true
        val textY = cx - (paint.descent() + paint.ascent()) / 2f
        canvas.drawText(number.toString(), cx, textY, paint)
        return bmp
    }

    // customRasterTileUrl/customRasterAboveLayerId (5.0.4, zero-dead-props
    // phase - previously stored-only on BOTH platforms since before 4.0.2).
    // Injects a raster tile source + layer into the map style, using the
    // Maps SDK's own style-extension DSL (bundled with the existing
    // com.mapbox.maps:android dependency). Idempotent: removes any previous
    // instance first, so it is safe to call on EVERY style load (both the
    // initial setupNavigation load and day/night style switches wipe
    // injected sources/layers) and on live prop changes; a null/empty URL
    // simply cleans up. Wrapped so a style failure can never break
    // navigation (cosmetic-only guarantee, same as the waypoint markers).
    private fun applyCustomRasterLayer(style: Style) {
        try {
            if (style.styleLayerExists(CUSTOM_RASTER_LAYER_ID)) style.removeStyleLayer(CUSTOM_RASTER_LAYER_ID)
            if (style.styleSourceExists(CUSTOM_RASTER_SOURCE_ID)) style.removeStyleSource(CUSTOM_RASTER_SOURCE_ID)
            val url = customRasterTileUrl?.takeIf { it.isNotEmpty() } ?: return
            style.addSource(rasterSource(CUSTOM_RASTER_SOURCE_ID) {
                tiles(listOf(url))
                tileSize(256L)
            })
            val layer = rasterLayer(CUSTOM_RASTER_LAYER_ID, CUSTOM_RASTER_SOURCE_ID) {}
            val above = customRasterAboveLayerId?.takeIf { it.isNotEmpty() }
            if (above != null && style.styleLayerExists(above)) {
                style.addLayerAbove(layer, above)
            } else {
                style.addLayer(layer)
            }
        } catch (e: Exception) {
            Log.e(TAG, "applyCustomRasterLayer failed: ${e.message}")
        }
    }

    private fun clearWaypointMarkers() {
        try {
            waypointAnnotationManager?.deleteAll()
        } catch (e: Exception) {
            Log.e(TAG, "clearWaypointMarkers failed: ${e.message}")
        }
    }

    private fun updateWaypointMarkers(route: NavigationRoute) {
        try {
            val manager = waypointAnnotationManager
                ?: mapView.annotations.createPointAnnotationManager().also { waypointAnnotationManager = it }
            manager.deleteAll()

            // OFFSET FIX (5.0.3): use the ROUTE-SNAPPED waypoint locations
            // from the Directions RESPONSE, not the raw request coordinates.
            // Per Mapbox's own DirectionsResponse docs, response waypoints
            // are "input coordinates snapped to the road and path network,
            // in the order of the input coordinates" - i.e. exactly where
            // the drawn route line starts/ends. The raw request coordinates
            // (a geocoded address) can sit tens of meters off the road, and
            // a real device screenshot showed the flag visibly detached
            // from the route line's endpoint because of it. Response
            // waypoints also only contain LEG-SEPARATING stops (silent
            // via-points are excluded by the API itself), so every
            // intermediate entry is a true numbered stop by construction.
            //
            // COMPILE FIX (5.0.5): `route.waypoints`, NOT
            // `route.directionsResponse.waypoints()` - the latter was the
            // Navigation SDK v2 shape and does not exist on v3's
            // NavigationRoute. Verified verbatim at v3.20.0 (the version
            // Android actually resolves): NavigationRoute.kt line 55
            // declares `val waypoints: List<DirectionsWaypoint>?` directly
            // on the route - same snapped response waypoints, one hop
            // shorter. Caught by the first real Android compile since
            // 5.0.2 (this package has no local Gradle; 5.0.3/5.0.4's
            // Android additions had only ever been source-verified).
            val snappedPoints: List<Point>? = try {
                route.waypoints?.mapNotNull { it.location() }
            } catch (e: Exception) {
                Log.e(TAG, "updateWaypointMarkers: snapped waypoints unavailable (${e.message}), falling back to raw coordinates")
                null
            }
            // Fallback (raw request coordinates + waypointIndices semantics,
            // the pre-5.0.3 behavior) only if the response carries no
            // waypoints - markers stay slightly offset there, but never
            // disappear entirely.
            val points: List<Point> = snappedPoints ?: coordinates.mapNotNull { coord ->
                val lat = coord["latitude"] ?: return@mapNotNull null
                val lng = coord["longitude"] ?: return@mapNotNull null
                Point.fromLngLat(lng, lat)
            }
            if (points.size < 2) return

            val markers = mutableListOf<PointAnnotationOptions>()
            points.forEachIndexed { index, point ->
                val isFinal = index == points.size - 1
                when {
                    isFinal -> markers.add(
                        PointAnnotationOptions()
                            .withPoint(point)
                            .withIconImage(drawDestinationFlagBitmap())
                            .withIconAnchor(IconAnchor.BOTTOM)
                    )
                    index > 0 -> markers.add(
                        PointAnnotationOptions()
                            .withPoint(point)
                            .withIconImage(drawWaypointBadgeBitmap(index))
                            .withIconAnchor(IconAnchor.CENTER)
                    )
                }
            }
            if (markers.isNotEmpty()) manager.create(markers)
        } catch (e: Exception) {
            // Marker rendering is cosmetic - it must never break navigation.
            Log.e(TAG, "updateWaypointMarkers failed: ${e.message}")
        }
    }

    private fun toggleNativeStepsPanel() {
        if (stepsPanelOverlay != null) {
            dismissNativeStepsPanel()
        } else {
            showNativeStepsPanel()
        }
    }

    private fun showNativeStepsPanel() {
        val root = rootLayout ?: return
        val activeRoute = mapboxNavigation?.getNavigationRoutes()?.firstOrNull() ?: return

        val viewHeight = if (height > 0) height else resources.displayMetrics.heightPixels
        val maxPanelHeight = (viewHeight * STEPS_PANEL_MAX_HEIGHT_FRACTION).toInt()

        val list = LinearLayout(context).apply {
            orientation = LinearLayout.VERTICAL
            setPadding((16 * dp).toInt(), (12 * dp).toInt(), (16 * dp).toInt(), (16 * dp).toInt())
        }

        var rowCount = 0
        activeRoute.directionsRoute.legs()?.forEach { leg ->
            leg.steps()?.forEach { step ->
                val instruction = step.maneuver()?.instruction() ?: ""
                if (instruction.isEmpty()) return@forEach
                if (rowCount > 0) {
                    list.addView(View(context).apply {
                        setBackgroundColor(Color.parseColor("#E0E0E0"))
                    }, LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 1))
                }
                list.addView(TextView(context).apply {
                    text = instruction
                    textSize = 16f
                    setTextColor(Color.parseColor("#1C1C1E"))
                    setPadding(0, (10 * dp).toInt(), 0, 0)
                })
                list.addView(TextView(context).apply {
                    text = formatStepDistance(step.distance() ?: 0.0)
                    textSize = 12f
                    setTextColor(Color.parseColor("#8E8E93"))
                    setPadding(0, (2 * dp).toInt(), 0, (10 * dp).toInt())
                })
                rowCount++
            }
        }
        if (rowCount == 0) return

        val scroll = MaxHeightScrollView(context, maxPanelHeight).apply {
            addView(list, FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.WRAP_CONTENT
            ))
            background = GradientDrawable().apply {
                setColor(Color.WHITE)
                // Rounded top corners only (order: TL, TL, TR, TR, BR, BR, BL, BL)
                cornerRadii = floatArrayOf(16 * dp, 16 * dp, 16 * dp, 16 * dp, 0f, 0f, 0f, 0f)
            }
            // Swallow taps on the panel itself so they don't reach the scrim
            // (which would dismiss it while the user is scrolling/reading).
            isClickable = true
        }

        val overlay = FrameLayout(context).apply {
            setBackgroundColor(Color.parseColor("#66000000"))
            setOnClickListener { dismissNativeStepsPanel() }
            addView(scroll, FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.BOTTOM
            ))
        }

        root.addView(overlay, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ))
        stepsPanelOverlay = overlay
    }

    private fun cancelNavigation() {
        speechApi.cancel()
        voiceInstructionsPlayer.clear()
        mapboxNavigation?.setNavigationRoutes(listOf())
        mapboxNavigation?.stopTripSession()
        firstLocationReceived = false
        isOverviewMode = false
        hideUI()
        onNavigationCancelled(mapOf<String, Any>())
    }

    private fun showUI() {
        maneuverView?.visibility = View.VISIBLE
        etaBar?.visibility = if (showEta) View.VISIBLE else View.INVISIBLE
        sideButtons?.visibility = View.VISIBLE
    }

    private fun hideUI() {
        // Close the native steps panel whenever navigation UI is torn down
        // (cancel, arrival teardown, etc.) - it must never outlive the
        // navigation session it lists steps for.
        dismissNativeStepsPanel()
        // Waypoint markers belong to the torn-down route - clear them too.
        clearWaypointMarkers()
        maneuverView?.visibility = View.INVISIBLE
        speedInfoView?.visibility = View.GONE
        etaBar?.visibility = View.INVISIBLE
        sideButtons?.visibility = View.INVISIBLE
    }

    private fun resolveVoiceUnits(): String {
        return when (voiceUnits?.lowercase()) {
            "metric" -> "metric"
            "imperial" -> "imperial"
            else -> {
                val locale = language?.let { Locale.forLanguageTag(it) } ?: Locale.getDefault()
                if (locale.country in setOf("US", "GB", "LR", "MM")) "imperial" else "metric"
            }
        }
    }

    @SuppressLint("MissingPermission")
    @OptIn(ExperimentalPreviewMapboxNavigationAPI::class)
    private fun fetchRoutes() {
        val nav = mapboxNavigation ?: return
        if (coordinates.size < 2) return
        // READINESS GUARD (5.0.3): never issue a route request before the
        // async style-load setup has fully completed (route line layers,
        // viewport data source, navigation camera, observers - see
        // setupNavigation). On fast devices the JS props/route flow can
        // beat the network-bound style load; requests fired in that window
        // produced a route the UI never drew (map stuck on world view,
        // camera never engaging - reported on recent Samsung/Xiaomi
        // hardware). The deferred fetch is replayed exactly once by the
        // setup callback when it finishes. The lateinit checks are
        // belt-and-suspenders alongside the flag: they make this guard
        // correct even if the flag's position in the setup flow ever
        // drifts again. Placed BEFORE the degenerate-route guard: its
        // free-drive fallback needs the same camera/session setup.
        if (!navigationSetupComplete || !::viewportDataSource.isInitialized || !::navigationCamera.isInitialized) {
            pendingRouteFetchUntilSetup = true
            return
        }

        // Degenerate-route guard (5.0.1, parity with iOS - same threshold,
        // same failure message): when EVERY coordinate lies within 25 m of
        // the first one, the whole trip is a single point - nothing to
        // navigate. Emits a clear, catchable onRoutesFailed instead of
        // starting a 0 m navigation session into an instant arrival screen.
        // Deliberately checks ALL coordinates against the first (not just
        // origin vs destination): a legitimate round trip (start -> distant
        // via point -> back to start) also has origin == destination and
        // must NOT be blocked. Uses the Android framework's own
        // Location.distanceBetween - no new dependency.
        val firstCoord = coordinates.first()
        val distanceResult = FloatArray(1)
        val wholeTripIsOnePoint = coordinates.all { coord ->
            android.location.Location.distanceBetween(
                firstCoord["latitude"] ?: 0.0, firstCoord["longitude"] ?: 0.0,
                coord["latitude"] ?: 0.0, coord["longitude"] ?: 0.0,
                distanceResult
            )
            distanceResult[0] < DEGENERATE_ROUTE_THRESHOLD_METERS
        }
        if (wholeTripIsOnePoint) {
            // Center the camera on the driver (the trip's first coordinate)
            // instead of leaving the map at world view (5.0.2): in this
            // branch no route is requested, so no trip session ever starts,
            // no enhanced locations flow, and the usual first-location
            // camera centering (safeCameraFollowing) never triggers - a
            // real device test showed the map stuck on a whole-world view.
            // The driver's position is already in hand as coordinates[0];
            // no location engine or permission dependency needed.
            val lat = firstCoord["latitude"]
            val lng = firstCoord["longitude"]
            if (lat != null && lng != null) {
                try {
                    mapView.mapboxMap.setCamera(
                        CameraOptions.Builder()
                            .center(Point.fromLngLat(lng, lat))
                            .zoom(DEGENERATE_ROUTE_CAMERA_ZOOM)
                            .build()
                    )
                } catch (e: Exception) {
                    Log.e(TAG, "degenerate-route camera centering failed: ${e.message}")
                }
            }
            // Free-drive session (5.0.2, parity with iOS's fallback map):
            // a trip session with NO routes = Mapbox's passive free-drive
            // mode. It makes enhanced locations flow, which animates the
            // puck AND triggers the existing first-location camera
            // centering (safeCameraFollowing) - so the static setCamera
            // above is instant feedback, and live following takes over on
            // the first GPS fix. If a real trip is requested later,
            // fetchRoutes' normal path (setNavigationRoutes +
            // startTripSession) transitions free drive to active guidance
            // seamlessly - the standard Android SDK flow this file already
            // uses. Location permission is guaranteed by the consuming
            // app's own gate before this view mounts (same assumption the
            // rest of this file already makes via @SuppressLint).
            try {
                nav.setNavigationRoutes(listOf())
                nav.startTripSession()
            } catch (e: Exception) {
                Log.e(TAG, "degenerate-route free-drive start failed: ${e.message}")
            }
            // `code` is a stable, machine-readable identifier: the consuming
            // app should switch on it to show its own LOCALIZED message
            // (the `message` string is English-only debugging text - this
            // package deliberately does not attempt to translate it; the
            // `language` prop only localizes what the Mapbox API generates,
            // never this package's own strings).
            onRoutesFailed(mapOf(
                "message" to DEGENERATE_ROUTE_MESSAGE,
                "code" to "same_location"
            ))
            return
        }
        val points = coordinates.map { Point.fromLngLat(it["longitude"] ?: 0.0, it["latitude"] ?: 0.0) }
        val locale = language?.let { Locale.forLanguageTag(it) } ?: Locale.getDefault()

        // useMapMatching (5.0.4, zero-dead-props phase - previously
        // stored-only on BOTH platforms): routes the request through the
        // Map Matching API via MapboxNavigation.requestMapMatching -
        // introduced with Core Framework 3.x. Verified verbatim at BOTH
        // v3.11.0 (the -ndk27 floor) and v3.20.0 (what Android actually
        // resolves today): MapMatchingOptions.Builder with coordinates/
        // profile/language/waypoints/bannerInstructions is identical at
        // both tags, and MapMatchingMatch.navigationRoute exists at both
        // (the result's `navigationRoutes` convenience getter does NOT -
        // removed between 3.11 and 3.20, see the success callback below).
        // Marked @ExperimentalPreviewMapboxNavigationAPI by Mapbox - the
        // @OptIn on this function acknowledges that. Success/cancel
        // handling mirrors the Directions callback exactly (same
        // destroyed-instance guard, same one-shot cancellation retry).
        // NOTE: excludeTypes/maxHeight/maxWidth are Directions-API-only
        // request parameters - the Map Matching API has no equivalents.
        if (useMapMatching) {
            try {
                val mmBuilder = MapMatchingOptions.Builder()
                    .coordinates(points)
                    .bannerInstructions(true)
                navigationProfile?.let { mmBuilder.profile(if (it.startsWith("mapbox/")) it else "mapbox/$it") }
                language?.let { mmBuilder.language(it) }
                waypointIndices?.let { mmBuilder.waypoints(it) }
                nav.requestMapMatching(mmBuilder.build(), object : MapMatchingAPICallback {
                    override fun success(result: MapMatchingSuccessfulResult) {
                        if (nav.isDestroyed) {
                            Log.w(TAG, "Map matching response arrived after MapboxNavigation was destroyed - ignoring")
                            return
                        }
                        // COMPILE FIX (5.0.5): the `navigationRoutes`
                        // convenience getter existed at v3.11.0 but was
                        // REMOVED from MapMatchingSuccessfulResult by
                        // v3.20.0 (the version Android actually resolves) -
                        // an @ExperimentalPreviewMapboxNavigationAPI is
                        // allowed to break like that. Only `matches`
                        // survives; MapMatchingMatch.navigationRoute exists
                        // at BOTH v3.11.0 and v3.20.0 (verified verbatim at
                        // each tag), so mapping over matches is the form
                        // portable across the supported version range.
                        val routes = result.matches.map { it.navigationRoute }
                        if (routes.isEmpty()) { onRoutesFailed(mapOf("message" to "No routes returned")); return }
                        nav.setNavigationRoutes(routes)
                        nav.startTripSession()
                        updateWaypointMarkers(routes.first())
                        routeCancelRetryUsed = false
                    }
                    override fun failure(failure: MapMatchingFailure) {
                        Log.e(TAG, "Map matching request failed: $failure")
                        onRoutesFailed(mapOf("message" to "Map matching request failed"))
                    }
                    override fun onCancel() {
                        if (nav.isDestroyed) return
                        Log.w(TAG, "Map matching request canceled by SDK")
                        if (!routeCancelRetryUsed && coordinates.size >= 2) {
                            routeCancelRetryUsed = true
                            fetchHandler.postDelayed({ scheduleFetchRoutes() }, 250L)
                        }
                    }
                })
            } catch (e: Exception) {
                Log.e(TAG, "Map matching request could not be issued: ${e.message}")
                onRoutesFailed(mapOf("message" to (e.message ?: "Map matching request failed")))
            }
            return
        }

        val builder = RouteOptions.builder()
            .applyDefaultNavigationOptions()
            .language(locale.toLanguageTag())
            .voiceUnits(resolveVoiceUnits())
            .coordinatesList(points)
            .annotations("maxspeed,congestion,duration,speed")
            // overview("full") is required by Mapbox support (GitHub issue #4069)
            // to maximize the coverage of maxspeed annotations in the response.
            // Without it, many road segments return no maxspeed data even when
            // the speed limit is known — confirmed via Mapbox Support correspondence.
            .overview("full")
            // ── FIX: Lane guidance was not displaying ───────────────────────────
            // Root cause: BannerInstructions.sub() (which carries lane data) is
            // only returned by the Directions API when explicitly requested.
            // applyDefaultNavigationOptions() does set bannerInstructions(true)
            // internally, but several confirmed cases (Mapbox GitHub issue #7377,
            // "Lane guidance not showing, using Navigation Drop-in UI") show this
            // needs to be set explicitly to reliably trigger lane data in the
            // response — official Mapbox examples do the same as a defensive fix.
            .bannerInstructions(true)
            // steps(true) is required for any BannerInstructions/VoiceInstructions
            // to be present in the response at all.
            .steps(true)
            // roundaboutExits(true) ensures lane + exit guidance is generated for
            // roundabout maneuvers specifically (separate flag from bannerInstructions).
            .roundaboutExits(true)
        waypointIndices?.let { builder.waypointIndicesList(it) }
        navigationProfile?.let { builder.profile(if (it.startsWith("mapbox/")) it else "mapbox/$it") }
        excludeTypes?.takeIf { it.isNotEmpty() }?.let { builder.exclude(it.joinToString(",")) }
        maxHeight?.let { builder.maxHeight(it) }
        maxWidth?.let { builder.maxWidth(it) }
        nav.requestRoutes(builder.build(), object : NavigationRouterCallback {
            override fun onRoutesReady(routes: List<NavigationRoute>, @RouterOrigin routerOrigin: String) {
                // DESTROYED-INSTANCE GUARD (5.0.3): route responses are
                // async - if the view unmounted meanwhile,
                // onDetachedFromWindow() has already called
                // MapboxNavigationProvider.destroy(), and ANY call on the
                // destroyed instance throws IllegalStateException ("This
                // instance of MapboxNavigation is destroyed") - a real
                // production crash with exactly this stack
                // (onRoutesReady -> startTripSession -> runIfNotDestroyed).
                if (nav.isDestroyed) {
                    Log.w(TAG, "Route response arrived after MapboxNavigation was destroyed - ignoring")
                    return
                }
                if (routes.isEmpty()) { onRoutesFailed(mapOf("message" to "No routes returned")); return }
                nav.setNavigationRoutes(routes)
                nav.startTripSession()
                updateWaypointMarkers(routes.first())
                // Success: refresh the one-shot cancellation-retry budget.
                routeCancelRetryUsed = false
            }
            override fun onFailure(reasons: List<RouterFailure>, routeOptions: RouteOptions) {
                // TRANSIENT-FAILURE RETRY (5.0.10): previously every
                // onFailure fired onRoutesFailed unconditionally, with no
                // retry - unlike onCanceled below, which already retried
                // once. This left a gap: a genuinely transient failure
                // reported via onFailure (not onCanceled) - e.g. a cold-
                // start connectivity blip - had no recovery path, matching
                // a real device report of the map staying stuck with no
                // route. RouterFailure.isRetryable is the SDK's OWN signal
                // for exactly this (verified verbatim in
                // mapbox-navigation-android's RouterFailure.kt at v3.20.0:
                // "Indicates if it makes sense to retry the failed route
                // request for this type of failure") - so this defers to
                // the SDK's own judgment rather than guessing which
                // RouterFailureType values are transient. Deliberately
                // narrow: only retries when the SDK itself says to, never
                // for permanent failures (bad input, no route found, auth
                // errors, missing tiles) - retrying those would just repeat
                // the same failure. Shares onCanceled's one-shot budget
                // (routeCancelRetryUsed) rather than a separate counter, so
                // the total retry allowance across BOTH failure paths
                // stays capped at one - no compounding retries.
                if (reasons.any { it.isRetryable } && !routeCancelRetryUsed && coordinates.size >= 2) {
                    routeCancelRetryUsed = true
                    Log.w(TAG, "Route request failed with a retryable reason (${reasons.firstOrNull()?.type}) - retrying once")
                    fetchHandler.postDelayed({ scheduleFetchRoutes() }, 250L)
                    return
                }
                onRoutesFailed(mapOf("message" to (reasons.firstOrNull()?.message ?: "Unknown error")))
            }
            override fun onCanceled(routeOptions: RouteOptions, @RouterOrigin routerOrigin: String) {
                // SDK-SIDE CANCELLATION RETRY (5.0.3): previously a silent
                // no-op - if the SDK cancelled the request (observed on
                // some recent devices), nothing reached JS and the map just
                // sat there. Retry ONCE per trip: the budget flag is only
                // reset by new coordinates or a successful response, never
                // here and never in fetchRoutes() - so a systematically
                // cancelling environment degrades to exactly one retry, not
                // an infinite 250ms loop.
                if (nav.isDestroyed) return
                Log.w(TAG, "Route request canceled by SDK (origin=$routerOrigin)")
                if (!routeCancelRetryUsed && coordinates.size >= 2) {
                    routeCancelRetryUsed = true
                    fetchHandler.postDelayed({ scheduleFetchRoutes() }, 250L)
                }
            }
        })
    }

    // ── Coalesced route refetching (5.0.0) ──────────────────────────────────
    // Every prop that fetchRoutes() actually reads now re-triggers a route
    // calculation when its value CHANGES (previously only `coordinates` did;
    // changing e.g. navigationProfile or language after mount was silently
    // ignored until the next coordinates change — stale-language voice
    // guidance was a concrete reported consequence). Two safeguards against
    // request storms / regressions:
    //   1. Equality guard in each setter — no refetch when RN re-delivers an
    //      identical value.
    //   2. Coalescing — triggers within the same main-looper turn merge into
    //      ONE deferred fetchRoutes() call (initial mount delivers all props
    //      in a single batch; without this, up to 8 simultaneous route
    //      requests would fire). Uses a main-Looper Handler, NOT View.post():
    //      View.post() defers queued runnables until the view is ATTACHED,
    //      and props are delivered before attachment — a Handler runs on the
    //      next main-looper turn regardless.
    // fetchRoutes() itself keeps its own guards (mapboxNavigation != null,
    // coordinates.size >= 2), so a scheduled fetch with no valid coordinates
    // is a harmless no-op. Props NOT read by fetchRoutes (mapStyle, mute,
    // useMapMatching, customRaster*) deliberately do NOT schedule a refetch.
    private val fetchHandler = android.os.Handler(android.os.Looper.getMainLooper())
    private var fetchScheduled = false
    // Set when fetchRoutes() was invoked before the async style-load setup
    // completed (see setupNavigation's loadStyle callback) - the fetch is
    // replayed exactly once when setup finishes, instead of firing a route
    // request that the not-yet-registered observers would mishandle.
    private var pendingRouteFetchUntilSetup = false
    // One-shot retry budget for SDK-side route request cancellations
    // (onCanceled). Reset ONLY when new coordinates arrive or a request
    // succeeds - deliberately NOT reset inside fetchRoutes() itself, which
    // would let a systematically-cancelling environment retry forever
    // (cancel -> retry -> fetch resets budget -> cancel -> retry -> ...).
    private var routeCancelRetryUsed = false
    private fun scheduleFetchRoutes() {
        if (fetchScheduled) return
        fetchScheduled = true
        fetchHandler.post {
            fetchScheduled = false
            fetchRoutes()
        }
    }

    fun setCoordinates(coords: List<Map<String, Double>>) { if (coords == coordinates) return; coordinates = coords; routeCancelRetryUsed = false; scheduleFetchRoutes() }
    fun setWaypointIndices(i: List<Int>?) { if (i == waypointIndices) return; waypointIndices = i; scheduleFetchRoutes() }
    fun setLanguage(l: String?) { if (l == language) return; language = l; scheduleFetchRoutes() }
    fun setVoiceUnits(u: String?) { if (u == voiceUnits) return; voiceUnits = u; scheduleFetchRoutes() }
    fun setNavigationProfile(p: String?) { if (p == navigationProfile) return; navigationProfile = p; scheduleFetchRoutes() }
    fun setExcludeTypes(t: List<String>?) { if (t == excludeTypes) return; excludeTypes = t; scheduleFetchRoutes() }
    fun setMapStyle(s: String?) { mapStyle = s }
    fun setMute(m: Boolean) { if (m != isMuted) toggleMute() }
    fun setMaxHeight(h: Double?) { if (h == maxHeight) return; maxHeight = h; scheduleFetchRoutes() }
    fun setMaxWidth(w: Double?) { if (w == maxWidth) return; maxWidth = w; scheduleFetchRoutes() }
    // Navigation icon position overrides (5.1.0) - camera framing only, never
    // affects the route itself, so these apply live to the already-computed
    // viewportDataSource instead of scheduling a refetch. Guarded by
    // ::viewportDataSource.isInitialized since these can arrive before
    // setupNavigation()'s async style load completes (see the readiness
    // guard elsewhere in this file for the same pattern) - in that case the
    // new value is simply picked up whenever followingPadding is next read,
    // once setup finishes.
    fun setNavigationViewportPaddingTop(v: Double?) {
        if (v == navigationViewportPaddingTop) return
        navigationViewportPaddingTop = v
        if (::viewportDataSource.isInitialized) viewportDataSource.followingPadding = followingPadding
    }
    fun setNavigationViewportPaddingLeft(v: Double?) {
        if (v == navigationViewportPaddingLeft) return
        navigationViewportPaddingLeft = v
        if (::viewportDataSource.isInitialized) viewportDataSource.followingPadding = followingPadding
    }
    fun setNavigationViewportPaddingBottom(v: Double?) {
        if (v == navigationViewportPaddingBottom) return
        navigationViewportPaddingBottom = v
        if (::viewportDataSource.isInitialized) viewportDataSource.followingPadding = followingPadding
    }
    fun setNavigationViewportPaddingRight(v: Double?) {
        if (v == navigationViewportPaddingRight) return
        navigationViewportPaddingRight = v
        if (::viewportDataSource.isInitialized) viewportDataSource.followingPadding = followingPadding
    }
    // useMapMatching switches which API the route request uses (5.0.4) -
    // it IS a route-affecting prop now, so changes re-trigger the fetch.
    fun setUseMapMatching(u: Boolean) { if (u == useMapMatching) return; useMapMatching = u; scheduleFetchRoutes() }
    // iOS-only prop (toggles NavigationViewController's built-in end-of-route
    // feedback screen). Android's custom-built navigation UI has no such
    // screen, so there is nothing to show or hide here - declared as a
    // stored no-op for cross-platform prop parity, matching this package's
    // existing convention for platform-specific props.
    fun setShowEndOfRouteFeedback(@Suppress("UNUSED_PARAMETER") show: Boolean) { /* no-op on Android */ }
    // Raster overlay props (5.0.4): applied live when the style is loaded;
    // always re-applied on style (re)loads via both loadStyle callbacks.
    fun setCustomRasterTileUrl(u: String?) {
        if (u == customRasterTileUrl) return
        customRasterTileUrl = u
        mapView.mapboxMap.getStyle()?.let { applyCustomRasterLayer(it) }
    }
    fun setCustomRasterAboveLayerId(l: String?) {
        if (l == customRasterAboveLayerId) return
        customRasterAboveLayerId = l
        mapView.mapboxMap.getStyle()?.let { applyCustomRasterLayer(it) }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Maneuver view creation/rebuild
    //
    // FIX for maneuverBackgroundColorDay/Night and maneuverTurnIconColor never
    // visibly applying: buildUI() runs in init{}, before Expo/RN has delivered
    // ANY props (props always arrive via setter calls made AFTER the native
    // view instance already exists). The color props are therefore always
    // still null the one time ManeuverViewOptions was previously built,
    // inline, in buildUI() — no matter what was set from JS. A prior fix
    // attempt tried calling `maneuverView?.setBackgroundColor(...)` from the
    // setter instead, but that colors the plain Android View background of
    // the outer MapboxManeuverView container — a different visual layer from
    // the SDK's own internal "maneuver background" panel that
    // ManeuverViewOptions.maneuverBackgroundColor actually controls, which
    // Mapbox draws on top of it — so visually nothing changed.
    //
    // The only way to actually apply new ManeuverViewOptions is to construct
    // a new MapboxManeuverView with them (ManeuverViewOptions is a
    // construction-time-only configuration, not something that can be
    // updated on a live view — confirmed against the public API surface).
    // So: color prop setters and checkAndSwitchDayNight() (which now also
    // affects this, since maneuverBackgroundColorNight needs to take over
    // once night mode is entered) all call rebuildManeuverView(), which
    // swaps in a freshly-configured view at the exact same position.
    // ─────────────────────────────────────────────────────────────────────────
    private fun resolveManeuverBackgroundColor(): String? =
        if (isNightMode) (maneuverBackgroundColorNight ?: maneuverBackgroundColorDay)
        else maneuverBackgroundColorDay

    private fun createManeuverView(): MapboxManeuverView {
        val maneuverOptionsBuilder = ManeuverViewOptions.Builder()
        val resolvedColor = resolveManeuverBackgroundColor()
        // DIAGNOSTIC (added while investigating a persistent report that this
        // color has no visible effect): confirms, via logcat, whether the
        // correct value is actually reaching this point at all. If this log
        // shows the CORRECT hex value every time yet the banner still shows
        // no color change, that would point to Mapbox's own SDK rendering
        // not respecting ManeuverViewOptions.maneuverBackgroundColor the way
        // its own documentation describes — a deeper issue this package
        // cannot fix by changing how it calls that same, confirmed-correct
        // API. If instead this logs null or a stale value, that confirms a
        // real data-flow bug on our side, which would be immediately
        // actionable. Search logcat for "ManeuverColorDebug".
        Log.d(TAG, "ManeuverColorDebug: resolvedColor=$resolvedColor isNightMode=$isNightMode maneuverBackgroundColorDay=$maneuverBackgroundColorDay maneuverBackgroundColorNight=$maneuverBackgroundColorNight")
        resolvedColor?.let { hex ->
            val parsed = parseColorSafe(hex)
            Log.d(TAG, "ManeuverColorDebug: parsed color for \"$hex\" = $parsed (null means parseColorSafe rejected it)")
            parsed?.let { maneuverOptionsBuilder.maneuverBackgroundColor(it) }
        }
        maneuverTurnIconColor?.let { hex ->
            parseColorSafe(hex)?.let { maneuverOptionsBuilder.turnIconManeuver(it) }
        }
        // Belt-and-suspenders attempt (see plugin/src/index.js's generation
        // of mapbox_maneuver_style.xml for the full reasoning): apply
        // MapboxCustomManeuverStyle, a style resource generated by this
        // package's config plugin ONLY when a maneuver background color is
        // configured, via ContextThemeWrapper — alongside the existing
        // ManeuverViewOptions approach above, not instead of it, since we
        // don't have certainty about which mechanism (if either) actually
        // controls the visible background in this SDK version.
        // getIdentifier() is the correct way to look up a resource by name
        // from a LIBRARY module — we can't reference the consuming app's
        // own generated R.style.MapboxCustomManeuverStyle directly at
        // compile time. Returns 0 (falsy) if the style doesn't exist
        // (e.g. no maneuver color configured at all), in which case we
        // safely fall back to the plain, unwrapped context — identical to
        // this package's behavior before this style existed.
        val styleResId = context.resources.getIdentifier(
            "MapboxCustomManeuverStyle", "style", context.packageName
        )
        Log.d(TAG, "ManeuverColorDebug: MapboxCustomManeuverStyle lookup returned resId=$styleResId (0 means not found/not generated)")
        val maneuverContext = if (styleResId != 0) {
            ContextThemeWrapper(context, styleResId)
        } else {
            context
        }
        val mv = MapboxManeuverView(maneuverContext, null, 0, maneuverOptionsBuilder.build())
        // Tap the instruction banner to see the full list of upcoming steps.
        // Two things happen on every tap, deliberately COEXISTING (5.0.1,
        // exact parity with iOS where the SDK's own built-in steps list and
        // the event both fire): the onManeuverBannerPressed event is emitted
        // with the full payload (the JS layer can keep rendering its own
        // sheet from it), AND the native steps panel below toggles.
        mv.setOnClickListener {
            emitFullRouteSteps()
            toggleNativeStepsPanel()
        }
        return mv
    }

    // Swaps the current maneuverView for a freshly-built one (with up-to-date
    // ManeuverViewOptions), preserving its exact position, layout params, and
    // visibility in the view hierarchy — a no-op visually beyond the actual
    // color change. Safe to call before buildUI() has run (does nothing yet,
    // buildUI() will pick up the current prop values when it runs) and safe
    // to call repeatedly (idempotent).
    private fun rebuildManeuverView() {
        val old = maneuverView
        if (old == null) {
            Log.d(TAG, "ManeuverColorDebug: rebuildManeuverView() called before buildUI() has run — no-op, will use current values when it does")
            return
        }
        val parent = old.parent as? FrameLayout
        if (parent == null) {
            Log.d(TAG, "ManeuverColorDebug: rebuildManeuverView() aborted — maneuverView's parent is not a FrameLayout (${old.parent?.javaClass?.name})")
            return
        }
        val index = parent.indexOfChild(old)
        val layoutParams = old.layoutParams
        val wasVisible = old.visibility
        parent.removeView(old)
        val fresh = createManeuverView()
        parent.addView(fresh as View, index, layoutParams)
        fresh.visibility = wasVisible
        maneuverView = fresh
        Log.d(TAG, "ManeuverColorDebug: rebuildManeuverView() completed successfully, view swapped")
    }

    fun setManeuverBackgroundColorDay(c: String?) {
        Log.d(TAG, "ManeuverColorDebug: setManeuverBackgroundColorDay called with \"$c\" (current field value: \"$maneuverBackgroundColorDay\")")
        if (c == maneuverBackgroundColorDay) {
            Log.d(TAG, "ManeuverColorDebug: setManeuverBackgroundColorDay — value unchanged, skipping rebuild")
            return
        }
        maneuverBackgroundColorDay = c
        rebuildManeuverView()
    }

    fun setManeuverBackgroundColorNight(c: String?) {
        if (c == maneuverBackgroundColorNight) return
        maneuverBackgroundColorNight = c
        rebuildManeuverView()
    }

    fun setManeuverTurnIconColor(c: String?) {
        if (c == maneuverTurnIconColor) return
        maneuverTurnIconColor = c
        rebuildManeuverView()
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Location puck (the icon showing the user's position/heading on the map)
    //
    // Unlike ManeuverViewOptions, LocationComponentPlugin's updateSettings {}
    // can be called again on the SAME already-created plugin instance at any
    // time — no view reconstruction needed here, just re-apply the settings.
    // mapView itself is a class-level property created immediately (not
    // inside init{}/buildUI()), so mapView.location is always safe to touch,
    // even before setupNavigation() has run.
    //
    // Default (navigationPuckColor == null) uses Mapbox's own stock
    // mapbox_navigation_puck_icon drawable completely unmodified — no
    // regression versus before this prop existed.
    // ─────────────────────────────────────────────────────────────────────────
    private fun tintedDrawableToBitmap(resId: Int, colorHex: String): Bitmap? {
        return try {
            val color = parseColorSafe(colorHex) ?: return null
            val drawable = ContextCompat.getDrawable(context, resId)?.mutate() ?: return null
            drawable.setTint(color)
            val width = drawable.intrinsicWidth.takeIf { it > 0 } ?: 1
            val height = drawable.intrinsicHeight.takeIf { it > 0 } ?: 1
            val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            val canvas = Canvas(bitmap)
            drawable.setBounds(0, 0, canvas.width, canvas.height)
            drawable.draw(canvas)
            bitmap
        } catch (e: Exception) {
            Log.e(TAG, "navigationPuckColor: failed to tint default icon: ${e.message}")
            null
        }
    }

    // Resolves a file:// URI or plain absolute path down to a filesystem
    // path, and loads it as a Bitmap. Returns null (never throws) on any
    // failure — missing file, unreadable, corrupted/unsupported image data,
    // permission error, etc. BitmapFactory.decodeFile itself already
    // returns null rather than throwing for most decode failures, but the
    // whole thing is wrapped regardless for full safety (e.g. malformed URI
    // parsing can throw).
    private fun loadBitmapFromPath(path: String): Bitmap? {
        return try {
            val resolvedPath = if (path.startsWith("file://")) {
                Uri.parse(path).path ?: return null
            } else {
                path
            }
            val file = File(resolvedPath)
            if (!file.exists() || !file.canRead()) {
                Log.e(TAG, "navigationPuckImagePath: file not found or unreadable: $resolvedPath")
                return null
            }
            BitmapFactory.decodeFile(resolvedPath)
        } catch (e: Exception) {
            Log.e(TAG, "navigationPuckImagePath: failed to load image: ${e.message}")
            null
        }
    }

    // Resolves the 2D puck image, in priority order:
    //   1. navigationPuckImagePath (custom image, used AS-IS — never tinted)
    //   2. navigationPuckColor (tints Mapbox's own default icon)
    //   3. Mapbox's own default icon, completely unmodified
    // Any failure at a given priority level (bad path, decode failure,
    // invalid color string) falls through to the next level rather than
    // leaving the puck without an image or crashing.
    private fun resolveBearingImageHolder(): ImageHolder {
        val defaultResId = com.mapbox.navigation.R.drawable.mapbox_navigation_puck_icon

        navigationPuckImagePath?.let { path ->
            loadBitmapFromPath(path)?.let { bitmap -> return ImageHolder.from(bitmap) }
        }

        navigationPuckColor?.let { colorHex ->
            tintedDrawableToBitmap(defaultResId, colorHex)?.let { bitmap -> return ImageHolder.from(bitmap) }
        }

        return ImageHolder.from(defaultResId)
    }

    // Attempts to build a LocationPuck3D from navigationPuck3DModelPath.
    // Returns null (never throws) if the path is a local file:// / absolute
    // path that doesn't actually exist on disk. "asset://" paths (Android's
    // own bundled assets/ folder) and http(s):// URLs are passed through
    // as-is to Mapbox's own modelUri — we can't cheaply pre-validate those
    // without a filesystem check or a network request, so those are left to
    // Mapbox's own loading; if Mapbox itself fails to load the model, per
    // its own documented/observed behavior this does not crash — the puck
    // simply doesn't render, at which point the recovery below in
    // applyPuckSettings() (falling back to the 2D puck if 3D produces no
    // visible result) is a best-effort mitigation, not a hard guarantee.
    // Validates that a local file genuinely starts with the .glb binary
    // format's required magic number (the ASCII bytes "glTF", 0x67 0x6C
    // 0x54 0x46 — per the official glTF 2.0 binary file format
    // specification's 12-byte header) BEFORE ever handing it to Mapbox's
    // renderer. Added after a reported crash with navigationPuck3DModelPath
    // — a malformed/non-glb file reaching the native 3D rendering pipeline
    // is a plausible cause of a crash that a Kotlin try/catch cannot
    // reliably catch (native/GPU-side rendering failures for a corrupt
    // asset are not necessarily raised as a catchable JVM exception, and
    // may happen asynchronously on a different thread than the one that
    // requested the puck configuration change). This is a cheap, fast
    // sanity check that rejects OBVIOUSLY invalid files outright; it does
    // not guarantee every possible malformed-but-magic-number-valid file
    // is safe, since fully validating a glTF/GLB file's internal
    // structure is a much larger undertaking than this check performs.
    private fun isValidGlbFile(path: String): Boolean {
        return try {
            val header = ByteArray(4)
            java.io.FileInputStream(path).use { stream ->
                val read = stream.read(header)
                if (read != 4) return false
            }
            // "glTF" in ASCII: 0x67 0x6C 0x54 0x46
            header[0] == 0x67.toByte() && header[1] == 0x6C.toByte() &&
                header[2] == 0x54.toByte() && header[3] == 0x46.toByte()
        } catch (e: Exception) {
            Log.e(TAG, "navigationPuck3DModelPath: failed to validate .glb header: ${e.message}")
            false
        }
    }

    private fun build3DPuck(path: String): LocationPuck3D? {
        return try {
            if (path.startsWith("file://") || (!path.contains("://"))) {
                val resolvedPath = if (path.startsWith("file://")) Uri.parse(path).path ?: return null else path
                if (!File(resolvedPath).exists()) {
                    Log.e(TAG, "navigationPuck3DModelPath: file not found: $resolvedPath")
                    return null
                }
                if (!isValidGlbFile(resolvedPath)) {
                    Log.e(TAG, "navigationPuck3DModelPath: file does not have a valid .glb header, refusing to use it: $resolvedPath")
                    return null
                }
                Log.d(TAG, "navigationPuck3DModelPath: .glb header validated OK, file size=${File(resolvedPath).length()} bytes: $resolvedPath")
            }
            // asset:// paths (Android bundled assets) were previously passed
            // through with NO validation at all - closed in 5.0.1: verify the
            // asset exists and starts with the same required .glb magic
            // number, via the standard AssetManager API (no new dependency).
            // A nonexistent or non-glb bundled asset reaching Mapbox's
            // native model loader is the same crash surface the file://
            // validation above already guards against.
            if (path.startsWith("asset://")) {
                val assetName = path.removePrefix("asset://")
                try {
                    context.assets.open(assetName).use { stream ->
                        val header = ByteArray(4)
                        val read = stream.read(header)
                        val isGlb = read == 4 &&
                            header[0] == 0x67.toByte() && header[1] == 0x6C.toByte() &&
                            header[2] == 0x54.toByte() && header[3] == 0x46.toByte()
                        if (!isGlb) {
                            Log.e(TAG, "navigationPuck3DModelPath: bundled asset is not a valid .glb, refusing to use it: $assetName")
                            return null
                        }
                    }
                } catch (e: Exception) {
                    Log.e(TAG, "navigationPuck3DModelPath: bundled asset not found/unreadable: $assetName (${e.message})")
                    return null
                }
            }
            // Remote (http/https) URLs can't be cheaply pre-validated this
            // way without downloading them first — passed through as-is to
            // Mapbox's own loading, same as before.
            val puck = LocationPuck3D(modelUri = path)
            // DIAGNOSTIC (reported crash, no log ever appears before it —
            // consistent with a native/GPU-side rendering failure that
            // bypasses Kotlin exception handling entirely, most likely
            // during the actual camera zoom/render pass rather than at
            // configuration time here). This log confirms the Kotlin side
            // successfully got this far; if the crash still shows no
            // Kotlin-level trace at all after this, that's further
            // evidence the failure is happening later, natively, outside
            // what any try/catch here can intercept.
            Log.d(TAG, "navigationPuck3DModelPath: LocationPuck3D object constructed successfully, handing off to Mapbox")
            puck
        } catch (e: Exception) {
            Log.e(TAG, "navigationPuck3DModelPath: failed to build 3D puck: ${e.message}")
            null
        }
    }

    // Applies the current puck configuration to the already-live
    // LocationComponentPlugin (mapView.location) — no view reconstruction
    // needed, unlike the maneuver banner. Safe to call at any time, even
    // before setupNavigation() has run (mapView itself is a class-level
    // property created immediately, not inside init{}).
    //
    // Precedence, per explicit design: 3D model (if set and valid) wins
    // outright over anything 2D — a 3D puck and a 2D image/color are
    // mutually exclusive, never combined. Wrapped in try/catch as a final
    // safety net around the actual SDK call itself, regardless of how
    // carefully the inputs were already validated above.
    private fun applyPuckSettings() {
        val puck3DPath = navigationPuck3DModelPath
        if (puck3DPath != null) {
            val puck3D = build3DPuck(puck3DPath)
            if (puck3D != null) {
                try {
                    mapView.location.updateSettings {
                        locationPuck = puck3D
                        puckBearingEnabled = true
                        enabled = true
                    }
                    return
                } catch (e: Exception) {
                    Log.e(TAG, "navigationPuck3DModelPath: SDK rejected 3D puck, falling back to 2D: ${e.message}")
                    // fall through to 2D below
                }
            }
        }

        try {
            mapView.location.updateSettings {
                locationPuck = LocationPuck2D(bearingImage = resolveBearingImageHolder())
                puckBearingEnabled = true
                enabled = true
            }
        } catch (e: Exception) {
            Log.e(TAG, "applyPuckSettings: failed to apply 2D puck settings: ${e.message}")
        }
    }

    fun setNavigationPuckColor(c: String?) {
        if (c == navigationPuckColor) return
        navigationPuckColor = c
        if (navigationSetupComplete) applyPuckSettings()
    }

    fun setNavigationPuckImagePath(p: String?) {
        if (p == navigationPuckImagePath) return
        navigationPuckImagePath = p
        if (navigationSetupComplete) applyPuckSettings()
    }

    fun setNavigationPuck3DModelPath(p: String?) {
        if (p == navigationPuck3DModelPath) return
        navigationPuck3DModelPath = p
        if (navigationSetupComplete) applyPuckSettings()
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Speed limit panel position
    //
    // "bottomLeft" (default) matches the original hardcoded position exactly
    // — 12dp/88dp margins, unchanged from before this prop existed, so
    // there's no visual change for anyone not setting this prop.
    //
    // "topRight" deliberately uses a wider right margin than "topLeft"'s
    // left margin — the side buttons (mute/overview/recenter) occupy the
    // top-right area already (see sideCol above, same 188dp top clearance),
    // so "topRight" pushes further left to sit clear of that button column
    // rather than overlapping it. This is an approximation based on the
    // buttons' own known size (56dp + margins), not a guarantee for every
    // possible screen size — "topLeft" or a bottom position avoid this
    // concern entirely, since nothing else occupies those areas.
    //
    // Layout params are updated in place on the existing live view
    // (layoutParams + requestLayout()) — no view reconstruction needed,
    // unlike the maneuver banner. Safe to call before buildUI() has run
    // (speedInfoView is still null then; buildUI() applies the current
    // value when it creates the view) and safe to call repeatedly.
    // ─────────────────────────────────────────────────────────────────────────
    private fun applySpeedLimitPosition() {
        val siv = speedInfoView ?: return
        val params = (siv.layoutParams as? FrameLayout.LayoutParams) ?: return
        // Real-device fix: add the system navigation/status bar inset on
        // top of each position's existing clearance, so this panel isn't
        // rendered under the system bar on real hardware — see the
        // WindowInsets listener set up in buildUI() for the full context.
        val bottomInset = lastSystemBarInsets.bottom
        val topInset = lastSystemBarInsets.top
        when (speedLimitPosition) {
            "bottomRight" -> {
                params.gravity = Gravity.BOTTOM or Gravity.END
                params.setMargins(0, 0, (12 * dp).toInt(), (88 * dp).toInt() + bottomInset)
            }
            "topLeft" -> {
                params.gravity = Gravity.TOP or Gravity.START
                params.setMargins((12 * dp).toInt(), (188 * dp).toInt() + topInset, 0, 0)
            }
            "topRight" -> {
                params.gravity = Gravity.TOP or Gravity.END
                // Wider right margin to clear the side button column (see note above).
                params.setMargins(0, (188 * dp).toInt() + topInset, (80 * dp).toInt(), 0)
            }
            else -> { // "bottomLeft" (default) — original position, unchanged
                params.gravity = Gravity.BOTTOM or Gravity.START
                params.setMargins((12 * dp).toInt(), 0, 0, (88 * dp).toInt() + bottomInset)
            }
        }
        siv.layoutParams = params
        siv.requestLayout()
    }

    fun setSpeedLimitPosition(p: String?) {
        val resolved = p ?: "bottomLeft"
        if (resolved == speedLimitPosition) return
        speedLimitPosition = resolved
        applySpeedLimitPosition()
    }

    fun setShowEta(show: Boolean?) {
        // FIX: was previously a non-nullable `Boolean` parameter — if Expo
        // Modules' behavior for an OMITTED optional boolean prop is to call
        // this setter with Kotlin's own default (false) rather than simply
        // not calling it at all, this would have silently forced showEta to
        // false immediately after view creation for anyone not explicitly
        // passing showEta={true} — a real, plausible explanation for the
        // ETA bar disappearing entirely, unrelated to route/navigation state
        // (confirmed separately: the maneuver banner works correctly with
        // real-time updates, so navigation genuinely is active — ruling out
        // "no route" as the cause). Treating null explicitly as "use the
        // default" removes this ambiguity regardless of Expo's exact
        // behavior here.
        val resolved = show ?: true
        showEta = resolved
        // If navigation is already active (maneuverView currently visible),
        // apply the change immediately rather than waiting for the next
        // showUI() call (e.g. the next route change).
        if (maneuverView?.visibility == View.VISIBLE) {
            etaBar?.visibility = if (resolved) View.VISIBLE else View.INVISIBLE
        }
    }

    fun setEtaBarBackgroundColor(c: String?) {
        etaBarBackgroundColor = c
        c?.let { hex ->
            parseColorSafe(hex)?.let { etaBar?.setBackgroundColor(it) }
        }
    }

    fun setEtaTextColor(c: String?) {
        etaTextColor = c
        c?.let { hex ->
            parseColorSafe(hex)?.let { color ->
                tvEtaTime?.setTextColor(color)
                tvDuration?.setTextColor(color)
            }
        }
    }

    fun setIconButtonColor(c: String?) {
        iconButtonColor = c
        // Redraw overview and recenter icons with new color
        btnOverviewView?.setImageBitmap(drawRouteOverviewIcon())
        // Recenter keeps its current alpha (0.4 following / 1.0 overview)
        val currentAlpha = btnRecenterView?.alpha ?: 0.4f
        btnRecenterView?.setImageBitmap(drawNavigationArrowIcon())
        btnRecenterView?.alpha = currentAlpha
        // Redraw mute icon only if not currently muted (muted uses iconButtonMutedColor)
        if (!isMuted) btnMuteView?.setImageBitmap(drawSpeakerIcon(false))
    }

    fun setIconButtonMutedColor(c: String?) {
        iconButtonMutedColor = c
        // Redraw mute icon only if currently muted
        if (isMuted) btnMuteView?.setImageBitmap(drawSpeakerIcon(true))
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        // FIX: every one of these is a `lateinit var`, only actually
        // assigned inside initAPIs()/setupNavigation() (called from init{}).
        // If this view gets detached before that setup has fully completed
        // — a hot-reload/fast-refresh remount in development, or Android
        // tearing down the Activity abnormally after some OTHER crash
        // elsewhere (e.g. mid-setup) — calling .cancel()/.shutdown() on an
        // uninitialized lateinit property throws
        // UninitializedPropertyAccessException, crashing the app a SECOND
        // time during cleanup itself, potentially masking whatever the
        // original problem was. Each call is now guarded with Kotlin's
        // standard ::property.isInitialized check — a no-op if setup never
        // got that far, instead of a crash.
        if (::speechApi.isInitialized) speechApi.cancel()
        if (::voiceInstructionsPlayer.isInitialized) voiceInstructionsPlayer.shutdown()
        if (::maneuverApi.isInitialized) maneuverApi.cancel()
        if (::routeLineApi.isInitialized) routeLineApi.cancel()
        if (::routeLineView.isInitialized) routeLineView.cancel()
        // OBSERVER TEARDOWN FIX (5.0.10): routesObserver/routeProgressObserver/
        // locationObserver were never unregistered here before (only
        // voiceInstructionsObserver was) - see the comment on routesObserver's
        // declaration above for the full reasoning and the real crash this
        // fixes. All four unregistered symmetrically, BEFORE destroy() (same
        // ordering already established for voiceInstructionsObserver), so no
        // stale callback can fire into a torn-down instance from here on.
        mapboxNavigation?.unregisterRoutesObserver(routesObserver)
        mapboxNavigation?.unregisterRouteProgressObserver(routeProgressObserver)
        mapboxNavigation?.unregisterLocationObserver(locationObserver)
        mapboxNavigation?.unregisterVoiceInstructionsObserver(voiceInstructionsObserver)
        mapboxNavigation?.stopTripSession()
        MapboxNavigationProvider.destroy()
        mapView.onStop()
        mapView.onDestroy()
    }
}
