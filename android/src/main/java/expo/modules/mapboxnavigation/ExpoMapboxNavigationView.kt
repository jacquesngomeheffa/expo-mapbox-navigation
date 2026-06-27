package expo.modules.mapboxnavigation

import android.content.Context
import android.view.ViewGroup
import com.mapbox.api.directions.v5.DirectionsCriteria
import com.mapbox.api.directions.v5.models.RouteOptions
import com.mapbox.geojson.Point
import com.mapbox.navigation.base.extensions.applyDefaultNavigationOptions
import com.mapbox.navigation.base.extensions.applyLanguageAndVoiceUnitOptions
import com.mapbox.navigation.base.route.NavigationRoute
import com.mapbox.navigation.base.route.NavigationRouterCallback
import com.mapbox.navigation.base.route.RouterFailure
import com.mapbox.navigation.base.route.RouterOrigin
import com.mapbox.navigation.base.trip.model.RouteProgress
import com.mapbox.navigation.base.trip.model.RouteProgressState
import com.mapbox.navigation.core.MapboxNavigation
import com.mapbox.navigation.core.MapboxNavigationProvider
import com.mapbox.navigation.core.directions.session.RoutesObserver
import com.mapbox.navigation.core.lifecycle.MapboxNavigationApp
import com.mapbox.navigation.core.trip.session.RouteProgressObserver
import com.mapbox.navigation.dropin.NavigationView
import com.mapbox.navigation.dropin.map.MapStyleLoader
import expo.modules.kotlin.AppContext
import expo.modules.kotlin.views.ExpoView
import java.util.Locale

class ExpoMapboxNavigationView(context: Context, appContext: AppContext) : ExpoView(context, appContext) {

  // ─── Props ──────────────────────────────────────────────────────────────
  var coordinates: List<Map<String, Double>> = emptyList()
  var waypointIndices: List<Int> = emptyList()
  var useRouteMatchingApi: Boolean = false
  var locale: String = ""
  var routeProfile: String = "driving-traffic"
  var routeExcludeList: List<String> = emptyList()
  var mapStyle: String? = null
  var mute: Boolean = false
  var vehicleMaxHeight: Double? = null
  var vehicleMaxWidth: Double? = null
  var customRasterSourceUrl: String? = null
  var placeCustomRasterLayerAbove: String? = null
  var disableAlternativeRoutes: Boolean = false
  var showsEndOfRouteFeedback: Boolean = false

  // ─── Events ─────────────────────────────────────────────────────────────
  private val onRouteProgressChanged by lazy { appContext.eventDispatcher("onRouteProgressChanged") }
  private val onWaypointArrival by lazy { appContext.eventDispatcher("onWaypointArrival") }
  private val onFinalDestinationArrival by lazy { appContext.eventDispatcher("onFinalDestinationArrival") }
  private val onCancelNavigation by lazy { appContext.eventDispatcher("onCancelNavigation") }
  private val onRouteChanged by lazy { appContext.eventDispatcher("onRouteChanged") }
  private val onUserOffRoute by lazy { appContext.eventDispatcher("onUserOffRoute") }
  private val onRoutesLoaded by lazy { appContext.eventDispatcher("onRoutesLoaded") }

  // ─── Internal ───────────────────────────────────────────────────────────
  private var navigationView: NavigationView? = null
  private var isStarted = false
  private var currentWaypointIndex = 0
  private var mapboxNavigation: MapboxNavigation? = null

  fun startNavigationIfReady() {
    if (coordinates.size >= 2 && !isStarted) {
      isStarted = true
      post { setupNavigationView() }
    }
  }

  private fun setupNavigationView() {
    val activity = context as? androidx.fragment.app.FragmentActivity ?: return

    // Créer la NavigationView (drop-in UI Mapbox)
    val navView = NavigationView(context).also {
      it.layoutParams = ViewGroup.LayoutParams(
        ViewGroup.LayoutParams.MATCH_PARENT,
        ViewGroup.LayoutParams.MATCH_PARENT
      )
    }
    navigationView = navView
    addView(navView)

    // Observer la progression
    navView.registerRouteProgressObserver(object : RouteProgressObserver {
      override fun onRouteProgressChanged(routeProgress: RouteProgress) {
        val progressData = mapOf(
          "distanceRemaining" to routeProgress.distanceRemaining,
          "distanceTraveled" to routeProgress.distanceTraveled,
          "durationRemaining" to routeProgress.durationRemaining,
          "fractionTraveled" to routeProgress.fractionTraveled
        )

        onRouteProgressChanged.emit(progressData)

        // Arrivée à un waypoint intermédiaire
        if (routeProgress.currentState == RouteProgressState.COMPLETE) {
          val legIndex = routeProgress.currentLegProgress?.legIndex ?: 0
          val totalLegs = routeProgress.navigationRoute?.directionsRoute?.legs()?.size ?: 1

          if (legIndex < totalLegs - 1) {
            onWaypointArrival.emit(progressData)
          } else {
            onFinalDestinationArrival.emit(emptyMap<String, Any>())
          }
        }
      }
    })

    // Observer les changements de route (reroutage)
    navView.registerRoutesObserver(object : RoutesObserver {
      override fun onRoutesChanged(result: RoutesObserver.RoutesUpdatedResult) {
        if (result.navigationRoutes.isNotEmpty()) {
          onRouteChanged.emit(emptyMap<String, Any>())
        }
      }
    })

    // Construire et lancer la route
    requestRoute(navView)
  }

  private fun requestRoute(navView: NavigationView) {
    val points = coordinates.mapNotNull { coord ->
      val lat = coord["latitude"] ?: return@mapNotNull null
      val lng = coord["longitude"] ?: return@mapNotNull null
      Point.fromLngLat(lng, lat)
    }
    if (points.size < 2) return

    val origin = points.first()
    val destination = points.last()
    val waypoints = if (points.size > 2) points.subList(1, points.size - 1) else emptyList()

    val resolvedLocale = if (locale.isNotEmpty()) Locale(locale) else Locale.getDefault()

    val profile = when (routeProfile) {
      "driving" -> DirectionsCriteria.PROFILE_DRIVING
      "walking" -> DirectionsCriteria.PROFILE_WALKING
      "cycling" -> DirectionsCriteria.PROFILE_CYCLING
      else -> DirectionsCriteria.PROFILE_DRIVING_TRAFFIC
    }

    val routeOptionsBuilder = RouteOptions.builder()
      .applyDefaultNavigationOptions(profile)
      .applyLanguageAndVoiceUnitOptions(context)
      .language(resolvedLocale.language)
      .coordinatesList(listOf(origin) + waypoints + listOf(destination))

    if (!disableAlternativeRoutes) {
      routeOptionsBuilder.alternatives(true)
    }

    if (routeExcludeList.isNotEmpty()) {
      routeOptionsBuilder.exclude(routeExcludeList.joinToString(","))
    }

    vehicleMaxHeight?.let { routeOptionsBuilder.maxHeight(it) }
    vehicleMaxWidth?.let { routeOptionsBuilder.maxWidth(it) }

    MapboxNavigationApp.current()?.requestRoutes(
      routeOptionsBuilder.build(),
      object : NavigationRouterCallback {
        override fun onRoutesReady(routes: List<NavigationRoute>, routerOrigin: RouterOrigin) {
          onRoutesLoaded.emit(emptyMap<String, Any>())
          MapboxNavigationApp.current()?.setNavigationRoutes(routes)
          navView.api.startActiveGuidance(routes)
        }

        override fun onFailure(reasons: List<RouterFailure>, routeOptions: RouteOptions) {
          android.util.Log.e("ExpoMapboxNavigation", "Erreur route: ${reasons.firstOrNull()?.message}")
        }

        override fun onCanceled(routeOptions: RouteOptions, routerOrigin: RouterOrigin) {}
      }
    )

    // Bouton d'annulation
    navView.addOnClickListener {
      onCancelNavigation.emit(emptyMap<String, Any>())
    }
  }

  override fun onDetachedFromWindow() {
    super.onDetachedFromWindow()
    MapboxNavigationApp.current()?.setNavigationRoutes(emptyList())
  }
}
