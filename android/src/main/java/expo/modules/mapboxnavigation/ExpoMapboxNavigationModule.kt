package expo.modules.mapboxnavigation

import expo.modules.kotlin.modules.Module
import expo.modules.kotlin.modules.ModuleDefinition

class ExpoMapboxNavigationModule : Module() {
    override fun definition() = ModuleDefinition {
        Name("ExpoMapboxNavigation")

        View(ExpoMapboxNavigationView::class) {
            // Events must be declared here — they correspond to EventDispatcher fields in the View
            Events(
                "onRouteProgressChanged",
                "onRoutesReady",
                "onNavigationFinished",
                "onNavigationCancelled",
                "onRoutesFailed",
                "onArrival",
                "onManeuverBannerPressed"
            )

            Prop("coordinates") { view: ExpoMapboxNavigationView, coordinates: List<Map<String, Double>> ->
                view.setCoordinates(coordinates)
            }
            Prop("waypointIndices") { view: ExpoMapboxNavigationView, indices: List<Int>? ->
                view.setWaypointIndices(indices)
            }
            Prop("language") { view: ExpoMapboxNavigationView, language: String? ->
                view.setLanguage(language)
            }
            Prop("voiceUnits") { view: ExpoMapboxNavigationView, units: String? ->
                view.setVoiceUnits(units)
            }
            Prop("navigationProfile") { view: ExpoMapboxNavigationView, profile: String? ->
                view.setNavigationProfile(profile)
            }
            Prop("excludeTypes") { view: ExpoMapboxNavigationView, types: List<String>? ->
                view.setExcludeTypes(types)
            }
            Prop("mapStyle") { view: ExpoMapboxNavigationView, style: String? ->
                view.setMapStyle(style)
            }
            Prop("mute") { view: ExpoMapboxNavigationView, mute: Boolean ->
                view.setMute(mute)
            }
            Prop("maxHeight") { view: ExpoMapboxNavigationView, height: Double? ->
                view.setMaxHeight(height)
            }
            Prop("maxWidth") { view: ExpoMapboxNavigationView, width: Double? ->
                view.setMaxWidth(width)
            }
            Prop("navigationViewportPaddingTop") { view: ExpoMapboxNavigationView, v: Double? ->
                view.setNavigationViewportPaddingTop(v)
            }
            Prop("navigationViewportPaddingLeft") { view: ExpoMapboxNavigationView, v: Double? ->
                view.setNavigationViewportPaddingLeft(v)
            }
            Prop("navigationViewportPaddingBottom") { view: ExpoMapboxNavigationView, v: Double? ->
                view.setNavigationViewportPaddingBottom(v)
            }
            Prop("navigationViewportPaddingRight") { view: ExpoMapboxNavigationView, v: Double? ->
                view.setNavigationViewportPaddingRight(v)
            }
            Prop("useMapMatching") { view: ExpoMapboxNavigationView, use: Boolean ->
                view.setUseMapMatching(use)
            }
            Prop("showEndOfRouteFeedback") { view: ExpoMapboxNavigationView, show: Boolean ->
                view.setShowEndOfRouteFeedback(show)
            }
            Prop("customRasterTileUrl") { view: ExpoMapboxNavigationView, url: String? ->
                view.setCustomRasterTileUrl(url)
            }
            Prop("customRasterAboveLayerId") { view: ExpoMapboxNavigationView, layerId: String? ->
                view.setCustomRasterAboveLayerId(layerId)
            }
            // ── Color customization props ────────────────────────────────────────
            Prop("maneuverBackgroundColorDay") { view: ExpoMapboxNavigationView, color: String? ->
                view.setManeuverBackgroundColorDay(color)
            }
            Prop("maneuverBackgroundColorNight") { view: ExpoMapboxNavigationView, color: String? ->
                view.setManeuverBackgroundColorNight(color)
            }
            Prop("maneuverTurnIconColor") { view: ExpoMapboxNavigationView, color: String? ->
                view.setManeuverTurnIconColor(color)
            }
            Prop("navigationPuckColor") { view: ExpoMapboxNavigationView, color: String? ->
                view.setNavigationPuckColor(color)
            }
            Prop("navigationPuckImagePath") { view: ExpoMapboxNavigationView, path: String? ->
                view.setNavigationPuckImagePath(path)
            }
            Prop("navigationPuck3DModelPath") { view: ExpoMapboxNavigationView, path: String? ->
                view.setNavigationPuck3DModelPath(path)
            }
            Prop("speedLimitPosition") { view: ExpoMapboxNavigationView, position: String? ->
                view.setSpeedLimitPosition(position)
            }
            Prop("showEta") { view: ExpoMapboxNavigationView, show: Boolean? ->
                view.setShowEta(show)
            }
            Prop("etaBarBackgroundColor") { view: ExpoMapboxNavigationView, color: String? ->
                view.setEtaBarBackgroundColor(color)
            }
            Prop("etaTextColor") { view: ExpoMapboxNavigationView, color: String? ->
                view.setEtaTextColor(color)
            }
            Prop("iconButtonColor") { view: ExpoMapboxNavigationView, color: String? ->
                view.setIconButtonColor(color)
            }
            Prop("iconButtonMutedColor") { view: ExpoMapboxNavigationView, color: String? ->
                view.setIconButtonMutedColor(color)
            }
        }
    }
}
