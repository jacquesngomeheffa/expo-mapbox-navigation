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

    // ── Viewport padding ──────────────────────────────────────────────────────
    private val followingPadding by lazy {
        EdgeInsets(180.0 * dp, 40.0 * dp, 300.0 * dp, 40.0 * dp)
    }
    private val overviewPadding by lazy {
        EdgeInsets(120.0 * dp, 40.0 * dp, 120.0 * dp, 40.0 * dp)
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

    init {
        buildUI()
        initAPIs()
        setupNavigation()
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Draw icons programmatically (matching screenshot: speaker, route, arrow)
    // ─────────────────────────────────────────────────────────────────────────

    private fun drawSpeakerIcon(muted: Boolean): Bitmap {
        val size = (44 * dp).toInt()
        val bmp = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        val c = Canvas(bmp)
        val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
            color = Color.BLACK
            style = Paint.Style.FILL
        }
        val s = size.toFloat()
        // Speaker body (trapezoid)
        val body = Path().apply {
            moveTo(s * 0.18f, s * 0.38f)
            lineTo(s * 0.38f, s * 0.38f)
            lineTo(s * 0.58f, s * 0.22f)
            lineTo(s * 0.58f, s * 0.78f)
            lineTo(s * 0.38f, s * 0.62f)
            lineTo(s * 0.18f, s * 0.62f)
            close()
        }
        c.drawPath(body, paint)
        if (!muted) {
            // Sound waves arcs
            paint.style = Paint.Style.STROKE
            paint.strokeWidth = s * 0.065f
            paint.strokeCap = Paint.Cap.ROUND
            // Inner arc
            c.drawArc(RectF(s*0.60f, s*0.33f, s*0.80f, s*0.67f), -60f, 120f, false, paint)
            // Outer arc
            c.drawArc(RectF(s*0.64f, s*0.24f, s*0.92f, s*0.76f), -55f, 110f, false, paint)
        } else {
            // X mark for muted
            paint.style = Paint.Style.STROKE
            paint.strokeWidth = s * 0.07f
            paint.strokeCap = Paint.Cap.ROUND
            paint.color = Color.parseColor("#FF4444")
            c.drawLine(s*0.65f, s*0.32f, s*0.88f, s*0.68f, paint)
            c.drawLine(s*0.88f, s*0.32f, s*0.65f, s*0.68f, paint)
        }
        return bmp
    }

    private fun drawRouteOverviewIcon(): Bitmap {
        val size = (44 * dp).toInt()
        val bmp = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        val c = Canvas(bmp)
        val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
            color = Color.BLACK
            style = Paint.Style.STROKE
            strokeWidth = size * 0.08f
            strokeCap = Paint.Cap.ROUND
            strokeJoin = Paint.Join.ROUND
        }
        val s = size.toFloat()
        // Route line with S-curve (like the image: start dot → curve → end pin)
        // Start dot
        paint.style = Paint.Style.FILL
        c.drawCircle(s * 0.28f, s * 0.75f, s * 0.07f, paint)
        // Route path
        paint.style = Paint.Style.STROKE
        val routePath = Path().apply {
            moveTo(s * 0.28f, s * 0.75f)
            cubicTo(
                s * 0.28f, s * 0.45f,
                s * 0.72f, s * 0.55f,
                s * 0.72f, s * 0.25f
            )
        }
        c.drawPath(routePath, paint)
        // End pin (circle with dot)
        paint.style = Paint.Style.STROKE
        c.drawCircle(s * 0.72f, s * 0.22f, s * 0.10f, paint)
        paint.style = Paint.Style.FILL
        c.drawCircle(s * 0.72f, s * 0.22f, s * 0.04f, paint)
        return bmp
    }

    private fun drawNavigationArrowIcon(): Bitmap {
        val size = (44 * dp).toInt()
        val bmp = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        val c = Canvas(bmp)
        val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
            color = Color.BLACK
            style = Paint.Style.FILL
        }
        val s = size.toFloat()
        // Navigation arrow pointing up (filled chevron/arrow like Waze)
        val arrow = Path().apply {
            moveTo(s * 0.50f, s * 0.14f)   // tip
            lineTo(s * 0.78f, s * 0.76f)   // bottom-right
            lineTo(s * 0.50f, s * 0.60f)   // bottom-center indent
            lineTo(s * 0.22f, s * 0.76f)   // bottom-left
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
        val mv = MapboxManeuverView(context)
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

        // Button 3: Recenter / Navigation arrow (hidden until in overview mode)
        val recenterBtn = makeIconButton(drawNavigationArrowIcon()) { recenterCamera() }
        recenterBtn.visibility = View.GONE
        sideCol.addView(recenterBtn, LinearLayout.LayoutParams(btnSize, btnSize))
        btnRecenterView = recenterBtn

        // ── SpeedInfoView — bottom-left, above ETA bar ────────────────────────
        val siv = MapboxSpeedInfoView(context)
        root.addView(siv as View, FrameLayout.LayoutParams(
            (72 * dp).toInt(),
            (72 * dp).toInt()
        ).also {
            it.gravity = Gravity.BOTTOM or Gravity.START
            it.setMargins((12 * dp).toInt(), 0, 0, (88 * dp).toInt())
        })
        siv.visibility = View.GONE
        speedInfoView = siv

        // ── ETA bottom bar ─────────────────────────────────────────────────────
        val bar = LinearLayout(context).apply {
            orientation = LinearLayout.HORIZONTAL
            setBackgroundColor(Color.parseColor("#1E2433"))
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
            textSize = 24f; setTextColor(Color.WHITE)
            typeface = android.graphics.Typeface.DEFAULT_BOLD
        }
        bar.addView(etaTime, LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f))
        tvEtaTime = etaTime

        val center = LinearLayout(context).apply {
            orientation = LinearLayout.VERTICAL; gravity = Gravity.CENTER
        }
        val dur = TextView(context).apply {
            textSize = 18f; setTextColor(Color.WHITE)
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

    // ── Voice callbacks (exact pattern from official TurnByTurnExperienceActivity) ──
    // SpeechError and SpeechValue are swapped vs v2: fold(error, value) in v3
    // speechCallback: Consumer<Expected<SpeechError, SpeechValue>>
    // Exact pattern from official TurnByTurnExperienceActivity
    // NOTE: in v3 fold order is (error, value) — SpeechError is left, SpeechValue is right
    private val speechCallback =
        com.mapbox.navigation.base.route.MapboxNavigationConsumer<
            com.mapbox.bindgen.Expected<SpeechError, SpeechValue>
        > { expected ->
            expected.fold(
                { error ->
                    // On-device TTS fallback when MP3 is not available
                    if (!isMuted) {
                        voiceInstructionsPlayer.play(
                            error.fallback,
                            voiceInstructionsPlayerCallback
                        )
                    }
                },
                { value ->
                    // Play the synthesized MP3 from Mapbox Voice API
                    if (!isMuted) {
                        voiceInstructionsPlayer.play(
                            value.announcement,
                            voiceInstructionsPlayerCallback
                        )
                    }
                }
            )
        }

    private val voiceInstructionsPlayerCallback =
        { announcement: SpeechAnnouncement ->
            // Cleanup the file after playing
            speechApi.clean(announcement)
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
                navigationLocationProvider.changePosition(location = loc, keyPoints = result.keyPoints)
                viewportDataSource.onLocationChanged(loc)
                viewportDataSource.evaluate()

                if (!firstLocationReceived) {
                    firstLocationReceived = true
                    safeCameraFollowing()
                }

                val fmtOptions = DistanceFormatterOptions.Builder(context).build()
                val speedInfo = speedInfoApi.updatePostedAndCurrentSpeed(result, fmtOptions)
                if (speedInfo != null) {
                    speedInfoView?.visibility = View.VISIBLE
                    speedInfoView?.render(speedInfo)
                } else {
                    speedInfoView?.visibility = View.GONE
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
            btnRecenterView?.visibility = View.VISIBLE
            try {
                navigationCamera.requestNavigationCameraToOverview()
            } catch (e: Exception) {
                Log.e(TAG, "Camera overview error: ${e.message}")
            }
        }
    }

    private fun recenterCamera() {
        isOverviewMode = false
        btnRecenterView?.visibility = View.GONE
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
