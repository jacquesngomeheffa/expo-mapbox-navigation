import ExpoModulesCore
import UIKit
import CoreLocation
// MapboxDirections types (Waypoint, NavigationRouteOptions, etc.) are
// re-exported by MapboxNavigationCore in Navigation SDK v3.
import MapboxNavigationCore
import MapboxNavigationUIKit
import MapboxMaps

// Parses a "#RRGGBB" or "#AARRGGBB" (with or without leading "#") hex string
// into a UIColor. Returns nil (never throws/crashes) for any invalid input —
// mirrors the try/catch-wrapped Color.parseColor(...) pattern used on
// Android for every color prop.
private func mapboxColor(fromHex hex: String) -> UIColor? {
    var hexSanitized = hex.trimmingCharacters(in: .whitespacesAndNewlines)
    hexSanitized = hexSanitized.replacingOccurrences(of: "#", with: "")
    var rgb: UInt64 = 0
    guard Scanner(string: hexSanitized).scanHexInt64(&rgb) else { return nil }
    switch hexSanitized.count {
    case 6:
        return UIColor(
            red:   CGFloat((rgb & 0xFF0000) >> 16) / 255.0,
            green: CGFloat((rgb & 0x00FF00) >> 8)  / 255.0,
            blue:  CGFloat(rgb & 0x0000FF) / 255.0,
            alpha: 1.0
        )
    case 8:
        return UIColor(
            red:   CGFloat((rgb & 0xFF000000) >> 24) / 255.0,
            green: CGFloat((rgb & 0x00FF0000) >> 16) / 255.0,
            blue:  CGFloat((rgb & 0x0000FF00) >> 8)  / 255.0,
            alpha: CGFloat(rgb & 0x000000FF) / 255.0
        )
    default:
        return nil
    }
}

// Resolves a "file://" URI, a plain absolute path, or a remote http(s)://
// URL string to a URL — used for both the custom puck image and the 3D
// model path. Returns nil (never throws) if a local path doesn't actually
// exist on disk; remote URLs are passed through as-is (can't cheaply
// pre-validate those without a network request).
private func resolveLocalOrRemoteURL(_ path: String) -> URL? {
    if path.hasPrefix("http://") || path.hasPrefix("https://") {
        return URL(string: path)
    }
    if path.hasPrefix("file://") {
        guard let url = URL(string: path) else { return nil }
        return FileManager.default.fileExists(atPath: url.path) ? url : nil
    }
    return FileManager.default.fileExists(atPath: path) ? URL(fileURLWithPath: path) : nil
}

// Loads a local image file safely — returns nil (never throws) on any
// failure (missing file, unreadable, corrupted/unsupported format).
// UIImage(contentsOfFile:) itself already returns nil rather than throwing
// for most decode failures, matching BitmapFactory.decodeFile's behavior
// on Android; the FileManager existence check below is an extra safety
// net before even attempting the decode.
private func loadUIImage(fromPath path: String) -> UIImage? {
    let resolvedPath: String
    if path.hasPrefix("file://") {
        guard let url = URL(string: path) else { return nil }
        resolvedPath = url.path
    } else {
        resolvedPath = path
    }
    guard FileManager.default.fileExists(atPath: resolvedPath) else { return nil }
    return UIImage(contentsOfFile: resolvedPath)
}

// MARK: - Custom day/night styles (maneuver banner background color)
//
// InstructionsBannerView is the confirmed v3 class name for the maneuver
// instructions banner (see the official "User interface | Navigation SDK
// v3 | iOS" guide, which uses this exact class in its own UIAppearance
// example). Styling must be applied inside Style.apply() (confirmed
// pattern from Mapbox's own "Maps for navigation" v3 guide, which shows
// the identical StandardDayStyle/StandardNightStyle subclassing approach
// used here). If neither maneuverBackgroundColorDay/Night is set, apply()
// only calls super.apply() — Mapbox's own default appearance, unchanged.
//
// NOTE: unlike Android (where the maneuver view can be rebuilt live at any
// time), these styles are only read once, when presentNavigationViewController()
// constructs a new NavigationViewController — NavigationOptions.styles is a
// construction-time configuration. Changing these colors while navigation
// is already active takes effect on the next route (the next time
// presentNavigationViewController() runs), not instantly. This is a real,
// architecture-driven difference from Android's live-updating behavior,
// not an oversight.
private final class ExpoManeuverDayStyle: StandardDayStyle {
    var maneuverBackgroundColor: UIColor?
    required init() {
        super.init()
        styleType = .day
    }
    override func apply() {
        super.apply()
        guard let color = maneuverBackgroundColor else { return }
        InstructionsBannerView.appearance(for: UITraitCollection(userInterfaceIdiom: .phone)).backgroundColor = color
        InstructionsBannerView.appearance(for: UITraitCollection(userInterfaceIdiom: .pad)).backgroundColor = color
    }
}

private final class ExpoManeuverNightStyle: StandardNightStyle {
    var maneuverBackgroundColor: UIColor?
    required init() {
        super.init()
        styleType = .night
    }
    override func apply() {
        super.apply()
        guard let color = maneuverBackgroundColor else { return }
        InstructionsBannerView.appearance(for: UITraitCollection(userInterfaceIdiom: .phone)).backgroundColor = color
        InstructionsBannerView.appearance(for: UITraitCollection(userInterfaceIdiom: .pad)).backgroundColor = color
    }
}

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
    private var maneuverBackgroundColorNight: String?
    private var maneuverTurnIconColor:      String?
    private var etaBarBackgroundColor:      String?
    private var etaTextColor:               String?
    private var iconButtonColor:            String?
    private var iconButtonMutedColor:       String?
    // Location puck (the icon showing position/heading on the map — distinct
    // from maneuverTurnIconColor, which is inside the instruction banner).
    // Precedence, matching Android exactly: 3D model > custom image (never
    // tinted) > color tint (of a system symbol, not Mapbox's own internal
    // default asset — see resolvePuckConfiguration() for why) > default.
    private var navigationPuckColor:         String?
    private var navigationPuckImagePath:     String?
    private var navigationPuck3DModelPath:   String?

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

        // FIXED: waypointIndices was declared/settable but never applied on
        // iOS (dead code) - Android drives MapboxDirections'
        // waypointIndicesList(...) with it (see ExpoMapboxNavigationView.kt).
        // On iOS, the equivalent concept is Waypoint.separatesLegs: a
        // waypoint with separatesLegs=false is a silent "via point" the
        // route passes through without stopping/splitting the route into a
        // new leg. First and last waypoints always separate legs
        // regardless (the Directions API requires this), matching the
        // implicit behavior of Android's index list.
        let waypoints = coordinates.enumerated().map { index, coord -> Waypoint in
            var wp = Waypoint(coordinate: CLLocationCoordinate2D(
                latitude:  coord["latitude"]  ?? 0.0,
                longitude: coord["longitude"] ?? 0.0
            ))
            if let indices = waypointIndices, !indices.isEmpty {
                wp.separatesLegs = indices.contains(index) || index == 0 || index == coordinates.count - 1
            }
            return wp
        }

        var options = NavigationRouteOptions(waypoints: waypoints)
        if let langTag = language { options.locale = Locale(identifier: langTag) }
        options.distanceMeasurementSystem = resolveVoiceUnits() == "imperial" ? .imperial : .metric

        switch navigationProfile ?? "driving-traffic" {
        case "driving-traffic": options.profileIdentifier = .automobileAvoidingTraffic
        case "driving":         options.profileIdentifier = .automobile
        case "walking":         options.profileIdentifier = .walking
        case "cycling":         options.profileIdentifier = .cycling
        default:                options.profileIdentifier = .automobileAvoidingTraffic
        }

        // FIXED: excludeTypes was declared/settable but never applied on
        // iOS (dead code) - Android drives MapboxDirections' exclude(...)
        // request param with it. RoadClasses(descriptions:) is
        // MapboxDirections' own confirmed public initializer for building
        // this option set from plain strings (e.g. "toll", "motorway",
        // "ferry" - the same values already accepted by the Android side
        // and documented in this package's Props table).
        if let excludes = excludeTypes, !excludes.isEmpty {
            options.roadClassesToAvoid = RoadClasses(descriptions: excludes)
        }

        // FIXED: maxHeight/maxWidth were declared/settable but never
        // applied on iOS (dead code) - Android drives MapboxDirections'
        // maxHeight(...)/maxWidth(...) request params with them (vehicle
        // dimension restrictions for truck routing, NOT view layout
        // sizing - confirmed against Android's own usage, which passes
        // these straight to its Directions request builder, not to any
        // view/layout API). RouteOptions.maximumHeight/maximumWidth are
        // typed as Measurement<UnitLength> (confirmed from
        // mapbox-directions-swift's own RouteOptions.swift, which converts
        // via `.converted(to: .meters)` before encoding) - Android's
        // values are already in meters (matching its own MapboxDirections
        // Java/Kotlin API), so no unit conversion is needed here.
        if let h = maxHeight {
            options.maximumHeight = Measurement(value: h, unit: UnitLength.meters)
        }
        if let w = maxWidth {
            options.maximumWidth = Measurement(value: w, unit: UnitLength.meters)
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

        // Custom day/night styles for the maneuver banner background color.
        // Read HERE, at construction time — see the note on
        // ExpoManeuverDayStyle/ExpoManeuverNightStyle above for why this
        // can't be updated live on an already-presented NavigationViewController
        // the way Android's maneuver view can.
        let dayStyle = ExpoManeuverDayStyle()
        dayStyle.maneuverBackgroundColor = maneuverBackgroundColorDay.flatMap { mapboxColor(fromHex: $0) }
        let nightStyle = ExpoManeuverNightStyle()
        nightStyle.maneuverBackgroundColor = maneuverBackgroundColorNight.flatMap { mapboxColor(fromHex: $0) }

        let navigationOptions = NavigationOptions(
            mapboxNavigation: mapboxNavigation,
            voiceController:  provider.routeVoiceController,
            eventsManager:    provider.eventsManager(),
            styles: [dayStyle, nightStyle]
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

        // FIXED: mapStyle was declared/settable but never applied on iOS
        // (dead code) - Android drives mapView.mapboxMap.loadStyle(...)
        // with it. Confirmed real, current API from Mapbox's own
        // mapbox-navigation-ios example source (Examples/
        // AdditionalExamples/Examples/Advanced.swift, main branch):
        // `navigationMapView.mapView.mapboxMap.loadStyle(StyleURI(rawValue:
        // styleUrl)!)`. Only called when mapStyle is explicitly set -
        // Mapbox's own "Maps for navigation" guide recommends customizing
        // the style through a UI Style subclass (see
        // ExpoManeuverDayStyle/NightStyle above) rather than calling
        // loadStyle directly, specifically for day/night-switch
        // consistency - but that mechanism has no public style-URL hook on
        // StandardDayStyle/StandardNightStyle in this SDK version, so a
        // direct loadStyle call is the confirmed-working option here. When
        // mapStyle is nil, this is skipped entirely and the existing
        // default Nav Day/Night style behavior (already working, untouched
        // by this fix) applies exactly as before.
        if let styleURLString = mapStyle, let styleURI = StyleURI(rawValue: styleURLString) {
            vc.navigationView.navigationMapView.mapView.mapboxMap.loadStyle(styleURI)
        }

        // Apply the current puck configuration now that navigationMapView
        // exists — picks up whatever navigationPuckColor/ImagePath/
        // 3DModelPath were already set (via props received before this
        // route was fetched).
        applyPuckSettings()
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
    func setManeuverBackgroundColorDay(_ c: String?)   { maneuverBackgroundColorDay = c }
    func setManeuverBackgroundColorNight(_ c: String?) { maneuverBackgroundColorNight = c }
    func setManeuverTurnIconColor(_ c: String?)      { maneuverTurnIconColor = c }
    func setEtaBarBackgroundColor(_ c: String?)      { etaBarBackgroundColor = c }
    func setEtaTextColor(_ c: String?)               { etaTextColor = c }
    func setIconButtonColor(_ c: String?)            { iconButtonColor = c }
    func setIconButtonMutedColor(_ c: String?)       { iconButtonMutedColor = c }

    // MARK: - Location puck (parity with Android navigationPuckColor/
    // navigationPuckImagePath/navigationPuck3DModelPath)
    //
    // NavigationMapView.puckType is a confirmed, official v3 public API
    // (see "Maps for navigation | Navigation SDK v3 | iOS" guide:
    // `navigationMapView.puckType = .puck3D(.navigationDefault)`), backed
    // by MapboxMaps' Puck2DConfiguration/Puck3DConfiguration structs (see
    // Mapbox's own "Simulate navigation"/localized SDK guides for the
    // confirmed init signatures used below). Unlike the maneuver banner
    // styles above, puckType is a plain mutable property on an existing
    // NavigationMapView — no reconstruction needed, and it CAN be updated
    // live while navigation is already active.
    //
    // Precedence, matching Android's implementation exactly: 3D model (if
    // set and valid) > custom 2D image (never tinted, even if a color is
    // also set — avoids any risk of a tint operation on an arbitrary
    // user-supplied image) > color tint > Mapbox's own default icon.
    //
    // NOTE on navigationPuckColor specifically: unlike Android (where
    // Mapbox's own default puck drawable resource name is a confirmed,
    // public identifier we can tint directly), there is no equivalent
    // confirmed public asset name for iOS's own built-in puck image in the
    // SDK's public API surface. Tinting an unknown/unconfirmed internal
    // asset would risk referencing something that doesn't exist. Instead,
    // color-only customization uses a standard system symbol
    // ("location.north.circle.fill", present on every iOS version this
    // package supports) as the tintable base — this achieves the same
    // *intent* (a colored puck) via a guaranteed-safe building block,
    // though its exact shape won't match Mapbox's own default icon.
    private func applyPuckSettings() {
        guard let vc = navigationViewController else { return }
        let mapView = vc.navigationView.navigationMapView

        if let modelPath = navigationPuck3DModelPath,
           let url = resolveLocalOrRemoteURL(modelPath) {
            let model = Model(uri: url, orientation: [0, 0, 0])
            mapView.puckType = .puck3D(Puck3DConfiguration(model: model))
            return
        }

        if let imagePath = navigationPuckImagePath,
           let image = loadUIImage(fromPath: imagePath) {
            mapView.puckType = .puck2D(Puck2DConfiguration(bearingImage: image))
            return
        }

        if let colorHex = navigationPuckColor,
           let color = mapboxColor(fromHex: colorHex),
           let symbolImage = UIImage(systemName: "location.north.circle.fill")?
               .withTintColor(color, renderingMode: .alwaysOriginal) {
            mapView.puckType = .puck2D(Puck2DConfiguration(bearingImage: symbolImage))
            return
        }

        mapView.puckType = .puck2D(Puck2DConfiguration())
    }

    func setNavigationPuckColor(_ c: String?) {
        guard c != navigationPuckColor else { return }
        navigationPuckColor = c
        applyPuckSettings()
    }

    func setNavigationPuckImagePath(_ p: String?) {
        guard p != navigationPuckImagePath else { return }
        navigationPuckImagePath = p
        applyPuckSettings()
    }

    func setNavigationPuck3DModelPath(_ p: String?) {
        guard p != navigationPuck3DModelPath else { return }
        navigationPuck3DModelPath = p
        applyPuckSettings()
    }

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
