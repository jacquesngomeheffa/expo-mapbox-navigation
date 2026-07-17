import ExpoModulesCore

public class ExpoMapboxNavigationModule: Module {
  public func definition() -> ModuleDefinition {
    Name("ExpoMapboxNavigation")

    View(ExpoMapboxNavigationView.self) {

      // ── Events (exact parity with Android module Events() list) ─────────────
      Events(
        "onRouteProgressChanged",
        "onRoutesReady",
        "onNavigationFinished",
        "onNavigationCancelled",
        "onRoutesFailed",
        "onArrival",
        "onManeuverBannerPressed"
      )

      // ── Base props ────────────────────────────────────────────────────────────
      Prop("coordinates") { (view: ExpoMapboxNavigationView, v: [[String: Double]]) in
        view.setCoordinates(v)
      }
      Prop("waypointIndices") { (view: ExpoMapboxNavigationView, v: [Int]?) in
        view.setWaypointIndices(v)
      }
      Prop("language") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setLanguage(v)
      }
      Prop("voiceUnits") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setVoiceUnits(v)
      }
      Prop("navigationProfile") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setNavigationProfile(v)
      }
      Prop("excludeTypes") { (view: ExpoMapboxNavigationView, v: [String]?) in
        view.setExcludeTypes(v)
      }
      Prop("mapStyle") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setMapStyle(v)
      }
      Prop("mute") { (view: ExpoMapboxNavigationView, v: Bool) in
        view.setMute(v)
      }
      Prop("maxHeight") { (view: ExpoMapboxNavigationView, v: Double?) in
        view.setMaxHeight(v)
      }
      Prop("maxWidth") { (view: ExpoMapboxNavigationView, v: Double?) in
        view.setMaxWidth(v)
      }
      Prop("useMapMatching") { (view: ExpoMapboxNavigationView, v: Bool) in
        view.setUseMapMatching(v)
      }
      Prop("customRasterTileUrl") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setCustomRasterTileUrl(v)
      }
      Prop("customRasterAboveLayerId") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setCustomRasterAboveLayerId(v)
      }
      Prop("showEndOfRouteFeedback") { (view: ExpoMapboxNavigationView, v: Bool) in
        view.setShowEndOfRouteFeedback(v)
      }
      Prop("showEta") { (view: ExpoMapboxNavigationView, v: Bool) in
        view.setShowEta(v)
      }

      // ── Color customization props (parity with Android) ───────────────────────
      Prop("maneuverBackgroundColorDay") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setManeuverBackgroundColorDay(v)
      }
      Prop("maneuverBackgroundColorNight") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setManeuverBackgroundColorNight(v)
      }
      Prop("maneuverTurnIconColor") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setManeuverTurnIconColor(v)
      }
      Prop("navigationPuckColor") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setNavigationPuckColor(v)
      }
      Prop("navigationPuckImagePath") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setNavigationPuckImagePath(v)
      }
      Prop("navigationPuck3DModelPath") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setNavigationPuck3DModelPath(v)
      }
      Prop("etaBarBackgroundColor") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setEtaBarBackgroundColor(v)
      }
      Prop("etaTextColor") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setEtaTextColor(v)
      }
      Prop("iconButtonColor") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setIconButtonColor(v)
      }
      Prop("iconButtonMutedColor") { (view: ExpoMapboxNavigationView, v: String?) in
        view.setIconButtonMutedColor(v)
      }
    }
  }
}
