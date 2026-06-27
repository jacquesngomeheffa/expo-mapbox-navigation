package expo.modules.mapboxnavigation

import expo.modules.kotlin.modules.Module
import expo.modules.kotlin.modules.ModuleDefinition

class ExpoMapboxNavigationModule : Module() {
  override fun definition() = ModuleDefinition {
    Name("ExpoMapboxNavigation")

    View(ExpoMapboxNavigationView::class) {

      // ─── Props ──────────────────────────────────────────────────────

      Prop("coordinates") { view: ExpoMapboxNavigationView, value: List<Map<String, Double>> ->
        view.coordinates = value
        view.startNavigationIfReady()
      }

      Prop("waypointIndices") { view: ExpoMapboxNavigationView, value: List<Int> ->
        view.waypointIndices = value
      }

      Prop("useRouteMatchingApi") { view: ExpoMapboxNavigationView, value: Boolean ->
        view.useRouteMatchingApi = value
      }

      Prop("locale") { view: ExpoMapboxNavigationView, value: String ->
        view.locale = value
      }

      Prop("routeProfile") { view: ExpoMapboxNavigationView, value: String ->
        // Android: sans le préfixe "mapbox/"
        view.routeProfile = value.removePrefix("mapbox/")
      }

      Prop("routeExcludeList") { view: ExpoMapboxNavigationView, value: List<String> ->
        view.routeExcludeList = value
      }

      Prop("mapStyle") { view: ExpoMapboxNavigationView, value: String ->
        view.mapStyle = value
      }

      Prop("mute") { view: ExpoMapboxNavigationView, value: Boolean ->
        view.mute = value
      }

      Prop("vehicleMaxHeight") { view: ExpoMapboxNavigationView, value: Double ->
        view.vehicleMaxHeight = value
      }

      Prop("vehicleMaxWidth") { view: ExpoMapboxNavigationView, value: Double ->
        view.vehicleMaxWidth = value
      }

      Prop("customRasterSourceUrl") { view: ExpoMapboxNavigationView, value: String ->
        view.customRasterSourceUrl = value
      }

      Prop("placeCustomRasterLayerAbove") { view: ExpoMapboxNavigationView, value: String ->
        view.placeCustomRasterLayerAbove = value
      }

      Prop("disableAlternativeRoutes") { view: ExpoMapboxNavigationView, value: Boolean ->
        view.disableAlternativeRoutes = value
      }

      Prop("showsEndOfRouteFeedback") { view: ExpoMapboxNavigationView, value: Boolean ->
        view.showsEndOfRouteFeedback = value
      }

      // ─── Events ─────────────────────────────────────────────────────

      Events(
        "onRouteProgressChanged",
        "onWaypointArrival",
        "onFinalDestinationArrival",
        "onCancelNavigation",
        "onRouteChanged",
        "onUserOffRoute",
        "onRoutesLoaded"
      )
    }
  }
}
