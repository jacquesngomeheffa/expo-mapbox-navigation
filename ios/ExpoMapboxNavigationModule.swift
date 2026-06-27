import ExpoModulesCore

public class ExpoMapboxNavigationModule: Module {
  public func definition() -> ModuleDefinition {
    Name("ExpoMapboxNavigation")

    View(ExpoMapboxNavigationView.self) {

      // ─── Props ────────────────────────────────────────────────────────

      Prop("coordinates") { (view: ExpoMapboxNavigationView, value: [[String: Double]]) in
        view.coordinates = value
        view.startNavigationIfReady()
      }

      Prop("waypointIndices") { (view: ExpoMapboxNavigationView, value: [Int]) in
        view.waypointIndices = value
      }

      Prop("useRouteMatchingApi") { (view: ExpoMapboxNavigationView, value: Bool) in
        view.useRouteMatchingApi = value
      }

      Prop("locale") { (view: ExpoMapboxNavigationView, value: String) in
        view.locale = value
      }

      Prop("routeProfile") { (view: ExpoMapboxNavigationView, value: String) in
        view.routeProfile = value
      }

      Prop("routeExcludeList") { (view: ExpoMapboxNavigationView, value: [String]) in
        view.routeExcludeList = value
      }

      Prop("mapStyle") { (view: ExpoMapboxNavigationView, value: String) in
        view.mapStyle = value
      }

      Prop("mute") { (view: ExpoMapboxNavigationView, value: Bool) in
        view.mute = value
      }

      Prop("vehicleMaxHeight") { (view: ExpoMapboxNavigationView, value: Double) in
        view.vehicleMaxHeight = value
      }

      Prop("vehicleMaxWidth") { (view: ExpoMapboxNavigationView, value: Double) in
        view.vehicleMaxWidth = value
      }

      Prop("customRasterSourceUrl") { (view: ExpoMapboxNavigationView, value: String) in
        view.customRasterSourceUrl = value
      }

      Prop("placeCustomRasterLayerAbove") { (view: ExpoMapboxNavigationView, value: String) in
        view.placeCustomRasterLayerAbove = value
      }

      Prop("disableAlternativeRoutes") { (view: ExpoMapboxNavigationView, value: Bool) in
        view.disableAlternativeRoutes = value
      }

      Prop("showsEndOfRouteFeedback") { (view: ExpoMapboxNavigationView, value: Bool) in
        view.showsEndOfRouteFeedback = value
      }

      // ─── Events ───────────────────────────────────────────────────────

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
