import ExpoModulesCore
import UIKit
import CoreLocation
// MapboxDirections types (Waypoint, NavigationRouteOptions, etc.) are
// re-exported by MapboxNavigationCore in Navigation SDK v3.
import MapboxNavigationCore
import MapboxNavigationUIKit

public class ExpoMapboxNavigationView: ExpoView {

    // MARK: - Events (mirrors Android EventDispatchers exactly)
    let onRouteProgressChanged = EventDispatcher()
    let onRoutesReady          = EventDispatcher()
    let onNavigationFinished   = EventDispatcher()
    let onNavigationCancelled  = EventDispatcher()
    let onRoutesFailed         = EventDispatcher()
    let onArrival              = EventDispatcher()
    let onManeuverBannerPressed = EventDispatcher()

    // MARK: - Mapbox core
    private var mapboxNavigationProvider: MapboxNavigationProvider?
    private var mapboxNavigation: MapboxNavigation?
    private var navigationViewController: NavigationViewController?
    private var currentNavigationRoutes: NavigationRoutes?
    private var routeRequestTask: Task<Void, Never>?

    // MARK: - State (mirrors Android state vars)
    private var isMuted        = false
    private var isOverviewMode = false

    // MARK: - Base props (parity with Android)
    private var coordinates:            [[String: Double]] = []
    private var waypointIndices:        [Int]?
    private var language:               String?
    private var voiceUnits:             String?
    private var navigationProfile:      String?
    private var excludeTypes:           [String]?
    private var mapStyle:               String?
    private var mute:                   Bool = false
    private var maxHeight:              Double?
    private var maxWidth:               Double?
    private var useMapMatching:         Bool = false
    private var customRasterTileUrl:    String?
    private var customRasterAboveLayerId: String?

    // MARK: - Color customization props (parity with Android)
    // On iOS the NavigationViewController drop-in handles all UI natively,
    // so we store these and apply what we can via its public API.
    private var maneuverBackgroundColorDay: String?
    private var maneuverTurnIconColor:      String?
    private var etaBarBackgroundColor:      String?
    private var etaTextColor:               String?
    private var iconButtonColor:            String?
    private var iconButtonMutedColor:       String?

    // MARK: - Init
    public required init(appContext: AppContext? = nil) {
        super.init(appContext: appContext)
        DispatchQueue.main.async { [weak self] in
            guard let self = self else { return }
            let provider = MapboxNavigationProvider(coreConfig: .init())
            self.mapboxNavigationProvider = provider
            self.mapboxNavigation = provider.mapboxNavigation
        }
    }

    // MARK: - Voice units (Issue #31 parity — exact same logic as Android)
    private func resolveVoiceUnits() -> String {
        if let units = voiceUnits?.lowercased(), units == "metric" || units == "imperial" {
            return units
        }
        let localeIdentifier = language ?? Locale.current.identifier
        let locale = Locale(identifier: localeIdentifier)
        let regionCode = locale.regionCode ?? ""
        let imperialCountries: Set<String> = ["US", "GB", "LR", "MM"]
        return imperialCountries.contains(regionCode) ? "imperial" : "metric"
    }

    // MARK: - Route fetching (parity with Android fetchRoutes)
    private func fetchRoutes() {
        guard coordinates.count >= 2, let mapboxNavigation = mapboxNavigation else { return }

        let waypoints = coordinates.map { coord -> Waypoint in
            Waypoint(coordinate: CLLocationCoordinate2D(
                latitude:  coord["latitude"]  ?? 0.0,
                longitude: coord["longitude"] ?? 0.0
            ))
        }

        var options = NavigationRouteOptions(waypoints: waypoints)
        if let langTag = language { options.locale = Locale(identifier: langTag) }
        options.distanceUnit = resolveVoiceUnits() == "imperial" ? .mile : .kilometer

        switch navigationProfile ?? "driving-traffic" {
        case "driving-traffic": options.profileIdentifier = .automobileAvoidingTraffic
        case "driving":         options.profileIdentifier = .automobile
        case "walking":         options.profileIdentifier = .walking
        case "cycling":         options.profileIdentifier = .cycling
        default:                options.profileIdentifier = .automobileAvoidingTraffic
        }

        routeRequestTask?.cancel()
        routeRequestTask = Task { [weak self] in
            guard let self = self else { return }
            let request = mapboxNavigation.routingProvider().calculateRoutes(options: options)
            switch await request.result {
            case .failure(let error):
                self.onRoutesFailed(["message": error.localizedDescription])
            case .success(let navigationRoutes):
                self.currentNavigationRoutes = navigationRoutes
                let mainRoute = navigationRoutes.mainRoute.route
                self.onRoutesReady([
                    "routeCount":      navigationRoutes.alternativeRoutes.count + 1,
                    "distanceMeters":  mainRoute.distance,
                    "durationSeconds": mainRoute.expectedTravelTime
                ])
                await MainActor.run {
                    self.presentNavigationViewController(with: navigationRoutes)
                }
            }
        }
    }

    // MARK: - Present NavigationViewController (drop-in: includes ETA bar,
    // speed limit, lane guidance, mute/overview/recenter buttons natively)
    private func presentNavigationViewController(with navigationRoutes: NavigationRoutes) {
        guard let provider = mapboxNavigationProvider,
              let mapboxNavigation = mapboxNavigation else { return }

        tearDownNavigationViewController()

        let navigationOptions = NavigationOptions(
            mapboxNavigation: mapboxNavigation,
            voiceController:  provider.routeVoiceController,
            eventsManager:    provider.eventsManager()
        )

        let vc = NavigationViewController(
            navigationRoutes:  navigationRoutes,
            navigationOptions: navigationOptions
        )
        vc.delegate = self
        vc.routeLineTracksTraversal = true

        // Apply mute state
        if mute {
            provider.routeVoiceController.speechSynthesizer.muted = true
        }

        // Add to view hierarchy FIRST, then attach tap handler
        addSubview(vc.view)
        vc.view.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            vc.view.topAnchor.constraint(equalTo: topAnchor),
            vc.view.bottomAnchor.constraint(equalTo: bottomAnchor),
            vc.view.leadingAnchor.constraint(equalTo: leadingAnchor),
            vc.view.trailingAnchor.constraint(equalTo: trailingAnchor)
        ])

        if let parentVC = findParentViewController() {
            parentVC.addChild(vc)
            vc.didMove(toParent: parentVC)
        }

        // Tap on banner → emit full steps list (parity with Android banner tap)
        attachManeuverBannerTapHandler(to: vc)

        self.navigationViewController = vc
        isOverviewMode = false
    }

    // MARK: - Banner tap handler (parity with Android mv.setOnClickListener)
    private func attachManeuverBannerTapHandler(to vc: NavigationViewController) {
        let tap = UITapGestureRecognizer(target: self, action: #selector(handleManeuverBannerTap))
        vc.navigationView.topBannerContainerView.addGestureRecognizer(tap)
        vc.navigationView.topBannerContainerView.isUserInteractionEnabled = true
    }

    @objc private func handleManeuverBannerTap() { emitFullRouteSteps() }

    // MARK: - Full route steps (parity with Android emitFullRouteSteps)
    private func emitFullRouteSteps() {
        guard let navigationRoutes = currentNavigationRoutes else {
            onManeuverBannerPressed(["steps": []])
            return
        }
        let route = navigationRoutes.mainRoute.route
        var stepsPayload: [[String: Any]] = []
        for leg in route.legs {
            for step in leg.steps {
                stepsPayload.append([
                    "instruction":      step.instructions,
                    "distanceMeters":   step.distance,
                    "durationSeconds":  step.expectedTravelTime,
                    "maneuverType":     String(describing: step.maneuverType),
                    "maneuverModifier": step.maneuverDirection.map { String(describing: $0) } ?? "",
                    "roadName":         step.names?.first ?? "",
                    "laneInstructions": [] // rendered natively by NavigationViewController banner
                ])
            }
        }
        onManeuverBannerPressed(["steps": stepsPayload])
    }

    // MARK: - Mute (parity with Android toggleMute)
    private func applyMute(_ shouldMute: Bool) {
        isMuted = shouldMute
        mapboxNavigationProvider?.routeVoiceController.speechSynthesizer.muted = shouldMute
    }

    // MARK: - Cancel (parity with Android cancelNavigation)
    private func cancelNavigation() {
        tearDownNavigationViewController()
        onNavigationCancelled([:])
    }

    // MARK: - Teardown
    private func tearDownNavigationViewController() {
        guard let vc = navigationViewController else { return }
        vc.willMove(toParent: nil)
        vc.view.removeFromSuperview()
        vc.removeFromParent()
        navigationViewController = nil
        currentNavigationRoutes  = nil
        isOverviewMode = false
    }

    private func findParentViewController() -> UIViewController? {
        var responder: UIResponder? = self
        while let r = responder {
            if let vc = r as? UIViewController { return vc }
            responder = r.next
        }
        return nil
    }

    // MARK: - Prop setters (exact parity with Android setters)
    func setCoordinates(_ coords: [[String: Double]]) {
        coordinates = coords
        if coords.count >= 2 { fetchRoutes() }
    }
    func setWaypointIndices(_ i: [Int]?)   { waypointIndices = i }
    func setLanguage(_ l: String?)         { language = l }
    func setVoiceUnits(_ u: String?)       { voiceUnits = u }
    func setNavigationProfile(_ p: String?) { navigationProfile = p }
    func setExcludeTypes(_ t: [String]?)   { excludeTypes = t }
    func setMapStyle(_ s: String?)         { mapStyle = s }
    func setMute(_ m: Bool)                { mute = m; applyMute(m) }
    func setMaxHeight(_ h: Double?)        { maxHeight = h }
    func setMaxWidth(_ w: Double?)         { maxWidth = w }
    func setUseMapMatching(_ u: Bool)      { useMapMatching = u }
    func setCustomRasterTileUrl(_ u: String?)        { customRasterTileUrl = u }
    func setCustomRasterAboveLayerId(_ l: String?)   { customRasterAboveLayerId = l }

    // Color props — stored for reference; NavigationViewController applies its own
    // theme automatically. Custom color support via NavigationViewController's
    // StyleManager or subclassing can be added in future iterations.
    func setManeuverBackgroundColorDay(_ c: String?) { maneuverBackgroundColorDay = c }
    func setManeuverTurnIconColor(_ c: String?)      { maneuverTurnIconColor = c }
    func setEtaBarBackgroundColor(_ c: String?)      { etaBarBackgroundColor = c }
    func setEtaTextColor(_ c: String?)               { etaTextColor = c }
    func setIconButtonColor(_ c: String?)            { iconButtonColor = c }
    func setIconButtonMutedColor(_ c: String?)       { iconButtonMutedColor = c }

    // MARK: - Lifecycle
    public override func removeFromSuperview() {
        routeRequestTask?.cancel()
        tearDownNavigationViewController()
        super.removeFromSuperview()
    }
}

// MARK: - NavigationViewControllerDelegate
// (parity with Android RoutesObserver + RouteProgressObserver + onArrival)
extension ExpoMapboxNavigationView: NavigationViewControllerDelegate {

    public func navigationViewController(
        _ navigationViewController: NavigationViewController,
        didUpdate progress: RouteProgress,
        with location: CLLocation,
        rawLocation: CLLocation
    ) {
        onRouteProgressChanged([
            "distanceRemaining":            progress.distanceRemaining,
            "durationRemaining":            progress.durationRemaining,
            "distanceTraveled":             progress.distanceTraveled,
            "fractionTraveled":             progress.fractionTraveled,
            "currentStepDistanceRemaining": progress.currentLegProgress.currentStepProgress.distanceRemaining
        ])
    }

    public func navigationViewController(
        _ navigationViewController: NavigationViewController,
        didArriveAt waypoint: Waypoint
    ) -> Bool {
        onArrival([:])
        return true // continue to next waypoint if multi-stop
    }

    public func navigationViewControllerDidDismiss(
        _ navigationViewController: NavigationViewController,
        byCanceling canceled: Bool
    ) {
        if canceled {
            cancelNavigation()
        } else {
            onNavigationFinished([:])
            tearDownNavigationViewController()
        }
    }
}
