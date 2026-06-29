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
                "onArrival"
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
            Prop("useMapMatching") { view: ExpoMapboxNavigationView, use: Boolean ->
                view.setUseMapMatching(use)
            }
            Prop("customRasterTileUrl") { view: ExpoMapboxNavigationView, url: String? ->
                view.setCustomRasterTileUrl(url)
            }
            Prop("customRasterAboveLayerId") { view: ExpoMapboxNavigationView, layerId: String? ->
                view.setCustomRasterAboveLayerId(layerId)
            }
        }
    }
}
