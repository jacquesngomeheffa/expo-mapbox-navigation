package expo.modules.mapboxnavigation

import android.annotation.SuppressLint
import android.content.Context
import android.content.res.Resources
import android.graphics.Color
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
import com.mapbox.maps.plugin.animation.camera
import com.mapbox.maps.plugin.locationcomponent.location
import com.mapbox.maps.plugin.locationcomponent.OnIndicatorBearingChangedListener
import com.mapbox.maps.plugin.locationcomponent.OnIndicatorPositionChangedListener
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

    // ── Side action buttons (mute, overview, recenter) ────────────────────────
    private var btnMute: TextView? = null
    private var btnOverview: TextView? = null
    private var btnRecenter: TextView? = null
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

    // ── State ─────────────────────────────────────────────────────────────────
    private var isNightMode = false
    private var isMuted = false
    private var isOverviewMode = false
    private var firstLocationReceived = false

    // ── Pixel density ─────────────────────────────────────────────────────────
    private val dp = context.resources.displayMetrics.density

    // ── Viewport padding (Waze-style: vehicle at ~30% from bottom) ────────────
    // top=180dp → room for maneuver banner
    // bottom=300dp → pushes vehicle UP toward 30% from bottom (Waze style)
    // Without large bottom padding, vehicle sits at screen center
    private val followingPadding by lazy {
        EdgeInsets(
            180.0 * dp,   // top (maneuver banner height)
            40.0 * dp,    // left
            300.0 * dp,   // bottom — large value pushes vehicle up like Waze
            40.0 * dp     // right
        )
    }
    private val overviewPadding by lazy {
        EdgeInsets(
            120.0 * dp,
            40.0 * dp,
            120.0 * dp,
            40.0 * dp
        )
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
    // Build Waze-style UI
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

        // ── ManeuverView — top banner ──────────────────────────────────────────
        val mv = MapboxManeuverView(context)
        root.addView(mv as View, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        ).also { it.gravity = Gravity.TOP })
        mv.visibility = View.INVISIBLE
        maneuverView = mv

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

        // ── Side action buttons (right side, Waze-style) ──────────────────────
        val sideCol = LinearLayout(context).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER_HORIZONTAL
            visibility = View.INVISIBLE
        }
        root.addView(sideCol, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.WRAP_CONTENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        ).also {
            it.gravity = Gravity.END or Gravity.BOTTOM
            it.setMargins(0, 0, (12 * dp).toInt(), (96 * dp).toInt())
        })
        sideButtons = sideCol

        // Mute button
        val muteBtn = makeCircleButton("🔊")
        muteBtn.setOnClickListener { toggleMute() }
        sideCol.addView(muteBtn, circleButtonParams())
        btnMute = muteBtn

        sideCol.addView(View(context), LinearLayout.LayoutParams(1, (10 * dp).toInt()))

        // Overview button
        val overviewBtn = makeCircleButton("⊕")
        overviewBtn.setOnClickListener { toggleOverview() }
        sideCol.addView(overviewBtn, circleButtonParams())
        btnOverview = overviewBtn

        sideCol.addView(View(context), LinearLayout.LayoutParams(1, (10 * dp).toInt()))

        // Recenter button (shown only in overview mode)
        val recenterBtn = makeCircleButton("◎")
        recenterBtn.setOnClickListener { recenterCamera() }
        recenterBtn.visibility = View.GONE
        sideCol.addView(recenterBtn, circleButtonParams())
        btnRecenter = recenterBtn

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

        // ETA arrival time
        val etaTime = TextView(context).apply {
            textSize = 24f
            setTextColor(Color.WHITE)
            typeface = android.graphics.Typeface.DEFAULT_BOLD
        }
        bar.addView(etaTime, LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f))
        tvEtaTime = etaTime

        // Duration + distance (center)
        val center = LinearLayout(context).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER
        }
        val dur = TextView(context).apply {
            textSize = 18f
            setTextColor(Color.WHITE)
            typeface = android.graphics.Typeface.DEFAULT_BOLD
            gravity = Gravity.CENTER
        }
        val dist = TextView(context).apply {
            textSize = 13f
            setTextColor(Color.parseColor("#AAAAAA"))
            gravity = Gravity.CENTER
        }
        center.addView(dur)
        center.addView(dist)
        bar.addView(center, LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 2f))
        tvDuration = dur
        tvDistance = dist

        // Cancel button
        val cancelBtn = TextView(context).apply {
            text = "✕"
            textSize = 22f
            setTextColor(Color.parseColor("#AAAAAA"))
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

    // Helper: creates a white circle button (Waze style)
    private fun makeCircleButton(text: String): TextView {
        return TextView(context).apply {
            this.text = text
            textSize = 20f
            gravity = Gravity.CENTER
            setTextColor(Color.BLACK)
            background = GradientDrawable().apply {
                shape = GradientDrawable.OVAL
                setColor(Color.WHITE)
                // Drop shadow via elevation
            }
            elevation = 6 * dp
        }
    }

    private fun circleButtonParams(): LinearLayout.LayoutParams {
        val size = (52 * dp).toInt()
        return LinearLayout.LayoutParams(size, size)
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

            // Camera viewport
            viewportDataSource = MapboxNavigationViewportDataSource(mapView.mapboxMap)
            viewportDataSource.followingPadding = followingPadding
            viewportDataSource.overviewPadding = overviewPadding

            navigationCamera = NavigationCamera(
                mapView.mapboxMap,
                mapView.camera,
                viewportDataSource
            )

            // ── Location puck — Waze-style arrow ─────────────────────────────
            // FIX: use mapbox_navigation_puck_icon (built-in arrow)
            // + PuckBearing.COURSE to orient it in direction of travel
            mapView.location.apply {
                setLocationProvider(navigationLocationProvider)
                updateSettings {
                    // Built-in navigation arrow (chevron) from Nav SDK
                    locationPuck = LocationPuck2D(
                        bearingImage = ImageHolder.from(
                            com.mapbox.navigation.R.drawable.mapbox_navigation_puck_icon
                        )
                    )
                    // COURSE = direction of movement (not compass heading)
                    puckBearingEnabled = true
                    enabled = true
                }
                // Must set puckBearing separately — confirmed from migration guide
                puckBearing = com.mapbox.maps.plugin.PuckBearing.COURSE
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
                    routeLineApi.setNavigationRoutes(result.navigationRoutes) { value ->
                        mapView.mapboxMap.style?.apply {
                            routeLineView.renderRouteDrawData(this, value)
                        }
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

            // Maneuver arrow on map
            mapView.mapboxMap.style?.let { style ->
                val arrowResult = routeArrowApi.addUpcomingManeuverArrow(routeProgress)
                routeArrowView.renderManeuverUpdate(style, arrowResult)
            }

            // Maneuver banner
            val maneuvers = maneuverApi.getManeuvers(routeProgress)
            maneuvers.fold(
                { error -> Log.w(TAG, "Maneuver error: ${error.errorMessage}"); Unit },
                { _ ->
                    maneuverView?.visibility = View.VISIBLE
                    maneuverView?.renderManeuvers(maneuvers)
                    Unit
                }
            )

            // Trip progress
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
                    (routeProgress.currentLegProgress
                        ?.currentStepProgress?.distanceRemaining ?: 0f)
            ))
        })

        // Location observer
        nav.registerLocationObserver(object : LocationObserver {
            override fun onNewRawLocation(rawLocation: Location) {}
            override fun onNewLocationMatcherResult(result: LocationMatcherResult) {
                val loc = result.enhancedLocation

                navigationLocationProvider.changePosition(
                    location = loc,
                    keyPoints = result.keyPoints,
                )

                viewportDataSource.onLocationChanged(loc)
                viewportDataSource.evaluate()

                // First GPS fix → enter following mode
                if (!firstLocationReceived) {
                    firstLocationReceived = true
                    safeCameraFollowing()
                }

                // Speed limit display
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
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Button actions
    // ─────────────────────────────────────────────────────────────────────────

    // Toggle mute voice guidance
    private fun toggleMute() {
        isMuted = !isMuted
        btnMute?.text = if (isMuted) "🔇" else "🔊"
        btnMute?.setTextColor(if (isMuted) Color.parseColor("#FF4444") else Color.BLACK)
        // Actual audio muting handled by the voice instruction observer in the app
        // We expose the state via event so the RN layer can mute TTS
        onNavigationCancelled(mapOf("type" to "mute", "muted" to isMuted))
    }

    // Toggle overview / following mode
    private fun toggleOverview() {
        if (isOverviewMode) {
            recenterCamera()
        } else {
            isOverviewMode = true
            btnOverview?.text = "◉"
            btnRecenter?.visibility = View.VISIBLE
            try {
                navigationCamera.requestNavigationCameraToOverview()
            } catch (e: Exception) {
                Log.e(TAG, "Camera overview error: ${e.message}")
            }
        }
    }

    // Recenter camera to vehicle (following mode)
    private fun recenterCamera() {
        isOverviewMode = false
        btnOverview?.text = "⊕"
        btnRecenter?.visibility = View.GONE
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
        val arrivalCal = Calendar.getInstance().apply {
            timeInMillis = estimatedTimeToArrivalMs
        }
        tvEtaTime?.text = String.format(
            "%02d:%02d",
            arrivalCal.get(Calendar.HOUR_OF_DAY),
            arrivalCal.get(Calendar.MINUTE)
        )

        val totalMin = (totalTimeRemainingSec / 60).toInt()
        tvDuration?.text = if (totalMin >= 60)
            "${totalMin / 60}h ${totalMin % 60}min"
        else
            "${totalMin} min"

        tvDistance?.text = if (distanceRemainingMetres >= 1000)
            String.format("%.1f km", distanceRemainingMetres / 1000.0)
        else
            "${distanceRemainingMetres.toInt()} m"
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
        val newStyle = if (shouldBeNight) NavigationStyles.NAVIGATION_NIGHT_STYLE
        else NavigationStyles.NAVIGATION_DAY_STYLE
        mapView.mapboxMap.loadStyle(newStyle) { style ->
            routeLineView.initializeLayers(style)
            mapboxNavigation?.getNavigationRoutes()?.takeIf { it.isNotEmpty() }?.let { routes ->
                routeLineApi.setNavigationRoutes(routes) { value ->
                    routeLineView.renderRouteDrawData(style, value)
                }
            }
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Camera following — issue #43 safe wrapper
    // ─────────────────────────────────────────────────────────────────────────
    private fun safeCameraFollowing() {
        try {
            navigationCamera.requestNavigationCameraToFollowing(
                stateTransitionOptions = NavigationCameraTransitionOptions.Builder()
                    .maxDuration(0)
                    .build()
            )
        } catch (e: Exception) {
            Log.e(TAG, "Camera following error: ${e.message}")
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Cancel navigation
    // ─────────────────────────────────────────────────────────────────────────
    private fun cancelNavigation() {
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

    // ─────────────────────────────────────────────────────────────────────────
    // Issue #31 — Voice units
    // ─────────────────────────────────────────────────────────────────────────
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

    // ─────────────────────────────────────────────────────────────────────────
    // Route request
    // ─────────────────────────────────────────────────────────────────────────
    @SuppressLint("MissingPermission")
    private fun fetchRoutes() {
        val nav = mapboxNavigation ?: return
        if (coordinates.size < 2) return

        val points = coordinates.map {
            Point.fromLngLat(it["longitude"] ?: 0.0, it["latitude"] ?: 0.0)
        }
        val locale = language?.let { Locale.forLanguageTag(it) } ?: Locale.getDefault()

        val builder = RouteOptions.builder()
            .applyDefaultNavigationOptions()
            .language(locale.toLanguageTag())
            .voiceUnits(resolveVoiceUnits())
            .coordinatesList(points)
            .annotations("maxspeed,congestion,duration,speed")

        waypointIndices?.let { builder.waypointIndicesList(it) }
        navigationProfile?.let {
            builder.profile(if (it.startsWith("mapbox/")) it else "mapbox/$it")
        }
        excludeTypes?.takeIf { it.isNotEmpty() }?.let {
            builder.exclude(it.joinToString(","))
        }
        maxHeight?.let { builder.maxHeight(it) }
        maxWidth?.let { builder.maxWidth(it) }

        nav.requestRoutes(builder.build(), object : NavigationRouterCallback {
            override fun onRoutesReady(
                routes: List<NavigationRoute>,
                @RouterOrigin routerOrigin: String
            ) {
                if (routes.isEmpty()) {
                    onRoutesFailed(mapOf("message" to "No routes returned"))
                    return
                }
                nav.setNavigationRoutes(routes)
                nav.startTripSession()
            }

            override fun onFailure(reasons: List<RouterFailure>, routeOptions: RouteOptions) {
                val msg = reasons.firstOrNull()?.message ?: "Unknown error"
                Log.e(TAG, "Route failed: $msg")
                onRoutesFailed(mapOf("message" to msg))
            }

            override fun onCanceled(
                routeOptions: RouteOptions,
                @RouterOrigin routerOrigin: String
            ) {
                Log.d(TAG, "Route cancelled")
            }
        })
    }

    // ── Prop setters ──────────────────────────────────────────────────────────
    fun setCoordinates(coords: List<Map<String, Double>>) {
        coordinates = coords
        if (coords.size >= 2) fetchRoutes()
    }
    fun setWaypointIndices(i: List<Int>?) { waypointIndices = i }
    fun setLanguage(l: String?) { language = l }
    fun setVoiceUnits(u: String?) { voiceUnits = u }
    fun setNavigationProfile(p: String?) { navigationProfile = p }
    fun setExcludeTypes(t: List<String>?) { excludeTypes = t }
    fun setMapStyle(s: String?) { mapStyle = s }
    fun setMute(m: Boolean) { mute = m; if (m != isMuted) toggleMute() }
    fun setMaxHeight(h: Double?) { maxHeight = h }
    fun setMaxWidth(w: Double?) { maxWidth = w }
    fun setUseMapMatching(u: Boolean) { useMapMatching = u }
    fun setCustomRasterTileUrl(u: String?) { customRasterTileUrl = u }
    fun setCustomRasterAboveLayerId(l: String?) { customRasterAboveLayerId = l }

    // ── Lifecycle ─────────────────────────────────────────────────────────────
    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        maneuverApi.cancel()
        routeLineApi.cancel()
        routeLineView.cancel()
        mapboxNavigation?.stopTripSession()
        MapboxNavigationProvider.destroy()
        mapView.onStop()
        mapView.onDestroy()
    }
}
