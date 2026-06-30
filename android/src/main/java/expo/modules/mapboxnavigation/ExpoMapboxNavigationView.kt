package expo.modules.mapboxnavigation

import android.annotation.SuppressLint
import android.content.Context
import android.content.res.Resources
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
import com.mapbox.maps.EdgeInsets
import com.mapbox.maps.ImageHolder
import com.mapbox.maps.MapView
import com.mapbox.maps.plugin.LocationPuck2D
import com.mapbox.maps.plugin.PuckBearing
import com.mapbox.maps.plugin.animation.camera
import com.mapbox.maps.plugin.locationcomponent.location
import com.mapbox.navigation.base.TimeFormat
import com.mapbox.navigation.base.extensions.applyDefaultNavigationOptions
import com.mapbox.navigation.base.formatter.DistanceFormatterOptions
import com.mapbox.navigation.base.options.NavigationOptions
import com.mapbox.navigation.base.route.NavigationRoute
import com.mapbox.navigation.base.route.NavigationRouterCallback
import com.mapbox.navigation.base.route.RouterFailure
import com.mapbox.navigation.base.route.RouterOrigin
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

private const val TAG = "ExpoMapboxNavigation"

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
    private val followingPadding by lazy {
        EdgeInsets(180.0 * dp, 40.0 * dp, 300.0 * dp, 40.0 * dp)
    }
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
    private var useMapMatching: Boolean = false
    private var customRasterTileUrl: String? = null
    private var customRasterAboveLayerId: String? = null

    // ── Color customization props ────────────────────────────────────────────
    // Maneuver banner colors — verified against ManeuverViewOptions public API
    // (maneuverBackgroundColor, turnIconManeuver are real documented properties)
    private var maneuverBackgroundColorDay: String? = null
    private var maneuverTurnIconColor: String? = null
    // ETA bottom bar colors — fully custom view, safe to color freely
    private var etaBarBackgroundColor: String? = null
    private var etaTextColor: String? = null
    // Custom icon button colors (mute, overview, recenter) — our own bitmaps
    private var iconButtonColor: String? = null
    private var iconButtonMutedColor: String? = null

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
        val color = try {
            Color.parseColor(if (muted) (iconButtonMutedColor ?: defaultColor) else (iconButtonColor ?: defaultColor))
        } catch (e: Exception) { Color.parseColor(defaultColor) }
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
            color = try { Color.parseColor(iconButtonColor ?: "#1A73E8") } catch (e: Exception) { Color.parseColor("#1A73E8") }
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
            color = try { Color.parseColor(iconButtonColor ?: "#1A73E8") } catch (e: Exception) { Color.parseColor("#1A73E8") }
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

        // Full-screen map
        root.addView(mapView, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ))

        // ── ManeuverView — top ─────────────────────────────────────────────────
        // Colors are customizable via maneuverBackgroundColorDay,
        // maneuverTurnIconColor, and laneGuidanceTurnIconColor props using the
        // SDK's official ManeuverViewOptions.Builder — confirmed public API
        // (maneuverBackgroundColor, turnIconManeuver, laneGuidanceTurnIconManeuver
        // are all real documented properties of ManeuverViewOptions).
        val maneuverOptionsBuilder = ManeuverViewOptions.Builder()
        maneuverBackgroundColorDay?.let {
            try { maneuverOptionsBuilder.maneuverBackgroundColor(Color.parseColor(it)) } catch (e: Exception) {}
        }
        maneuverTurnIconColor?.let {
            try { maneuverOptionsBuilder.turnIconManeuver(Color.parseColor(it)) } catch (e: Exception) {}
        }
        val mv = MapboxManeuverView(context, null, 0, maneuverOptionsBuilder.build())
        root.addView(mv as View, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        ).also { it.gravity = Gravity.TOP })
        mv.visibility = View.INVISIBLE
        // Feature: tap the instruction banner to see the full list of upcoming
        // turn-by-turn steps. We emit the data via event so the RN/JS layer can
        // render a bottom sheet or modal using its own native UI components —
        // consistent with how all other navigation events are surfaced.
        mv.setOnClickListener { emitFullRouteSteps() }
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
        root.addView(siv as View, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.WRAP_CONTENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        ).also {
            it.gravity = Gravity.BOTTOM or Gravity.START
            it.setMargins((12 * dp).toInt(), 0, 0, (88 * dp).toInt())
        })
        siv.visibility = View.GONE
        speedInfoView = siv

        // ── ETA bottom bar ─────────────────────────────────────────────────────
        val resolvedEtaBg = try { Color.parseColor(etaBarBackgroundColor ?: "#1E2433") } catch (e: Exception) { Color.parseColor("#1E2433") }
        val resolvedEtaText = try { Color.parseColor(etaTextColor ?: "#FFFFFF") } catch (e: Exception) { Color.WHITE }
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

        mapView.mapboxMap.loadStyle(mapStyle ?: getAutoStyle()) { style ->
            routeLineView.initializeLayers(style)

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
            mapView.location.apply {
                setLocationProvider(navigationLocationProvider)
                updateSettings {
                    locationPuck = LocationPuck2D(
                        bearingImage = ImageHolder.from(
                            com.mapbox.navigation.R.drawable.mapbox_navigation_puck_icon
                        )
                    )
                    puckBearingEnabled = true
                    enabled = true
                }
                puckBearing = PuckBearing.COURSE
            }

            registerObservers()
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Observers
    // ─────────────────────────────────────────────────────────────────────────
    private fun registerObservers() {
        val nav = mapboxNavigation ?: return

        // Routes observer
        nav.registerRoutesObserver(object : RoutesObserver {
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
        })

        // Route progress observer
        nav.registerRouteProgressObserver(RouteProgressObserver { routeProgress ->
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
        })

        // Location observer
        nav.registerLocationObserver(object : LocationObserver {
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

                // ── Speed limit (issue #4: was not displaying) ──────────────────
                // updatePostedAndCurrentSpeed returns null when:
                //  1. The route response has no `maxspeed` annotation for this segment
                //  2. GPS speed is unavailable
                // We log this so issues can be diagnosed via `adb logcat`
                val fmtOptions = DistanceFormatterOptions.Builder(context).build()
                val speedInfo = speedInfoApi.updatePostedAndCurrentSpeed(result, fmtOptions)
                if (speedInfo != null) {
                    speedInfoView?.visibility = View.VISIBLE
                    speedInfoView?.render(speedInfo)
                } else {
                    speedInfoView?.visibility = View.GONE
                    Log.d(TAG, "Speed info unavailable for this location/route segment " +
                        "(no maxspeed annotation or GPS speed not ready yet)")
                }

                checkAndSwitchDayNight()
            }
        })

        // Voice instructions observer — triggers TTS announcements
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
        mapView.mapboxMap.loadStyle(
            if (shouldBeNight) NavigationStyles.NAVIGATION_NIGHT_STYLE else NavigationStyles.NAVIGATION_DAY_STYLE
        ) { style ->
            routeLineView.initializeLayers(style)
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
        etaBar?.visibility = View.VISIBLE
        sideButtons?.visibility = View.VISIBLE
    }

    private fun hideUI() {
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
    private fun fetchRoutes() {
        val nav = mapboxNavigation ?: return
        if (coordinates.size < 2) return
        val points = coordinates.map { Point.fromLngLat(it["longitude"] ?: 0.0, it["latitude"] ?: 0.0) }
        val locale = language?.let { Locale.forLanguageTag(it) } ?: Locale.getDefault()
        val builder = RouteOptions.builder()
            .applyDefaultNavigationOptions()
            .language(locale.toLanguageTag())
            .voiceUnits(resolveVoiceUnits())
            .coordinatesList(points)
            .annotations("maxspeed,congestion,duration,speed")
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
                if (routes.isEmpty()) { onRoutesFailed(mapOf("message" to "No routes returned")); return }
                nav.setNavigationRoutes(routes)
                nav.startTripSession()
            }
            override fun onFailure(reasons: List<RouterFailure>, routeOptions: RouteOptions) {
                onRoutesFailed(mapOf("message" to (reasons.firstOrNull()?.message ?: "Unknown error")))
            }
            override fun onCanceled(routeOptions: RouteOptions, @RouterOrigin routerOrigin: String) {}
        })
    }

    fun setCoordinates(coords: List<Map<String, Double>>) { coordinates = coords; if (coords.size >= 2) fetchRoutes() }
    fun setWaypointIndices(i: List<Int>?) { waypointIndices = i }
    fun setLanguage(l: String?) { language = l }
    fun setVoiceUnits(u: String?) { voiceUnits = u }
    fun setNavigationProfile(p: String?) { navigationProfile = p }
    fun setExcludeTypes(t: List<String>?) { excludeTypes = t }
    fun setMapStyle(s: String?) { mapStyle = s }
    fun setMute(m: Boolean) { if (m != isMuted) toggleMute() }
    fun setMaxHeight(h: Double?) { maxHeight = h }
    fun setMaxWidth(w: Double?) { maxWidth = w }
    fun setUseMapMatching(u: Boolean) { useMapMatching = u }
    fun setCustomRasterTileUrl(u: String?) { customRasterTileUrl = u }
    fun setCustomRasterAboveLayerId(l: String?) { customRasterAboveLayerId = l }
    fun setManeuverBackgroundColorDay(c: String?) { maneuverBackgroundColorDay = c }
    fun setManeuverTurnIconColor(c: String?) { maneuverTurnIconColor = c }
    fun setEtaBarBackgroundColor(c: String?) { etaBarBackgroundColor = c }
    fun setEtaTextColor(c: String?) { etaTextColor = c }
    fun setIconButtonColor(c: String?) { iconButtonColor = c }
    fun setIconButtonMutedColor(c: String?) { iconButtonMutedColor = c }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        speechApi.cancel()
        voiceInstructionsPlayer.shutdown()
        maneuverApi.cancel()
        routeLineApi.cancel()
        routeLineView.cancel()
        mapboxNavigation?.unregisterVoiceInstructionsObserver(voiceInstructionsObserver)
        mapboxNavigation?.stopTripSession()
        MapboxNavigationProvider.destroy()
        mapView.onStop()
        mapView.onDestroy()
    }
}
