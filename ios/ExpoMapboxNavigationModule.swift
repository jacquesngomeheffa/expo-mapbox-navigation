import ExpoModulesCore

public class ExpoMapboxNavigationModule: Module {
  public func definition() -> ModuleDefinition {
    Name("ExpoMapboxNavigation")

    View(ExpoMapboxNavigationView.self) {
      // Events must exactly mirror the Android module's Events() list.
      Events(
        "onRouteProgressChanged",
        "onRoutesReady",
        "onNavigationFinished",
        "onNavigationCancelled",
        "onRoutesFailed",
        "onArrival",
        "onManeuverBannerPressed"
      )

      Prop("coordinates") { (view: ExpoMapboxNavigationView, coordinates: [[String: Double]]) in
        view.setCoordinates(coordinates)
      }
      Prop("waypointIndices") { (view: ExpoMapboxNavigationView, indices: [Int]?) in
        view.setWaypointIndices(indices)
      }
      Prop("language") { (view: ExpoMapboxNavigationView, language: String?) in
        view.setLanguage(language)
      }
      Prop("voiceUnits") { (view: ExpoMapboxNavigationView, units: String?) in
        view.setVoiceUnits(units)
      }
      Prop("navigationProfile") { (view: ExpoMapboxNavigationView, profile: String?) in
        view.setNavigationProfile(profile)
      }
      Prop("excludeTypes") { (view: ExpoMapboxNavigationView, types: [String]?) in
        view.setExcludeTypes(types)
      }
      Prop("mapStyle") { (view: ExpoMapboxNavigationView, style: String?) in
        view.setMapStyle(style)
      }
      Prop("mute") { (view: ExpoMapboxNavigationView, mute: Bool) in
        view.setMute(mute)
      }
      Prop("maxHeight") { (view: ExpoMapboxNavigationView, height: Double?) in
        view.setMaxHeight(height)
      }
      Prop("maxWidth") { (view: ExpoMapboxNavigationView, width: Double?) in
        view.setMaxWidth(width)
      }
      Prop("useMapMatching") { (view: ExpoMapboxNavigationView, use: Bool) in
        view.setUseMapMatching(use)
      }
      Prop("customRasterTileUrl") { (view: ExpoMapboxNavigationView, url: String?) in
        view.setCustomRasterTileUrl(url)
      }
      Prop("customRasterAboveLayerId") { (view: ExpoMapboxNavigationView, layerId: String?) in
        view.setCustomRasterAboveLayerId(layerId)
      }
    }
  }
}
