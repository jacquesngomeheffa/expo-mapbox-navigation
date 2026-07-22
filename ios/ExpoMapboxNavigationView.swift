import ExpoModulesCore
import UIKit
import CoreLocation
import Combine
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
// Shared appearance recipe for this package's custom color props (5.0.4,
// "zero dead props" phase). Every appearance target below is copied
// VERBATIM from the SDK's own DayStyle.swift at v3.20.1 - the SDK's own
// complete styling recipe - so these are guaranteed-valid appearance
// classes/properties, not guesses:
//   - ManeuverView.primaryColor/secondaryColor (@objc dynamic, lines 10-16
//     of ManeuverView.swift) -> maneuverTurnIconColor
//   - BottomBannerView.backgroundColor (DayStyle line 399) -> etaBarBackgroundColor
//   - TimeRemainingLabel.normalTextColor + its 5 traffic*Color variants,
//     DistanceRemainingLabel.normalTextColor, ArrivalTimeLabel.normalTextColor
//     (DayStyle lines 294-299, 353, 405) -> etaTextColor (traffic variants
//     deliberately unified to the custom color so the label never flips
//     back to SDK traffic colors)
//   - FloatingButton.tintColor (DayStyle line 345) -> iconButtonColor
// Applied inside Style.apply() at NavigationViewController construction,
// exactly like the existing maneuverBackgroundColor - construction-time
// semantics, documented since 4.0.2.
private func applyExpoCustomColors(
    maneuverBackground: UIColor?,
    turnIcon: UIColor?,
    etaBarBackground: UIColor?,
    etaText: UIColor?,
    iconButton: UIColor?
) {
    let idioms: [UIUserInterfaceIdiom] = [.phone, .pad]
    for idiom in idioms {
        let traits = UITraitCollection(userInterfaceIdiom: idiom)
        if let color = maneuverBackground {
            InstructionsBannerView.appearance(for: traits).backgroundColor = color
        }
        if let color = turnIcon {
            ManeuverView.appearance(for: traits).primaryColor = color
            ManeuverView.appearance(for: traits).secondaryColor = color
        }
        if let color = etaBarBackground {
            BottomBannerView.appearance(for: traits).backgroundColor = color
        }
        if let color = etaText {
            TimeRemainingLabel.appearance(for: traits).normalTextColor = color
            TimeRemainingLabel.appearance(for: traits).trafficHeavyColor = color
            TimeRemainingLabel.appearance(for: traits).trafficLowColor = color
            TimeRemainingLabel.appearance(for: traits).trafficModerateColor = color
            TimeRemainingLabel.appearance(for: traits).trafficSevereColor = color
            TimeRemainingLabel.appearance(for: traits).trafficUnknownColor = color
            DistanceRemainingLabel.appearance(for: traits).normalTextColor = color
            ArrivalTimeLabel.appearance(for: traits).normalTextColor = color
        }
        if let color = iconButton {
            FloatingButton.appearance(for: traits).tintColor = color
        }
    }
}

private final class ExpoManeuverDayStyle: StandardDayStyle {
    var maneuverBackgroundColor: UIColor?
    var turnIconColor: UIColor?
    var etaBarBackgroundColor: UIColor?
    var etaTextColor: UIColor?
    var iconButtonColor: UIColor?
    required init() {
        super.init()
        styleType = .day
    }
    override func apply() {
        super.apply()
        applyExpoCustomColors(
            maneuverBackground: maneuverBackgroundColor,
            turnIcon: turnIconColor,
            etaBarBackground: etaBarBackgroundColor,
            etaText: etaTextColor,
            iconButton: iconButtonColor
        )
    }
}

// Empty bottom banner (5.0.3): passed to NavigationOptions(bottomBanner:)
// when showEta == false, replacing the SDK's default BottomBannerViewController
// (the ETA/duration/distance bar + cancel button). This is the SDK's own
// supported customization point - NavigationViewController.addBottomBanner
// uses `navigationOptions?.bottomBanner ?? BottomBannerViewController()`
// (verified verbatim at v3.20.1, NavigationViewController.swift line 611),
// and Mapbox's own CustomBars example subclasses ContainerViewController
// exactly like this. Deterministic by construction - no show/hide timing
// races against the SDK's own banner presentation flow.
private final class ExpoEmptyBottomBanner: ContainerViewController {}

private final class ExpoManeuverNightStyle: StandardNightStyle {
    var maneuverBackgroundColor: UIColor?
    var turnIconColor: UIColor?
    var etaBarBackgroundColor: UIColor?
    var etaTextColor: UIColor?
    var iconButtonColor: UIColor?
    required init() {
        super.init()
        styleType = .night
    }
    override func apply() {
        super.apply()
        applyExpoCustomColors(
            maneuverBackground: maneuverBackgroundColor,
            turnIcon: turnIconColor,
            etaBarBackground: etaBarBackgroundColor,
            etaText: etaTextColor,
            iconButton: iconButtonColor
        )
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
    // Free-drive fallback map (5.0.2): shown when the degenerate-route guard
    // fires (driver already at destination) - a standalone NavigationMapView
    // in a passive free-drive session, so the driver sees a live map
    // centered on themselves instead of a black view.
    private var freeDriveMapView: NavigationMapView?
    // Waypoint markers (5.0.2): keeps the destination-flag image registered
    // across style (re)loads on the drop-in map - style changes wipe
    // registered images, so this observer re-adds it every time a style
    // finishes loading (the exact pattern of Mapbox's own v3.20.1
    // Custom-Final-Waypoint example). Cancelled on teardown.
    private var waypointImageStyleCancelable: Cancelable?

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
    // Navigation icon (puck) position overrides (5.1.0) - each independently
    // optional, in points. nil (the default for all four) means "use the
    // SDK's own existing default" (UIEdgeInsets(top: 20, left: 20,
    // bottom: 40, right: 20), verified verbatim in NavigationMapView.swift
    // at v3.20.1 - this package never overrode it before, so preserving
    // that default here is required for no-regression). Maps 1:1 to
    // NavigationMapView.viewportPadding, the SDK's own public property for
    // this - not a custom mechanism.
    private var navigationViewportPaddingTop:    Double?
    private var navigationViewportPaddingLeft:   Double?
    private var navigationViewportPaddingBottom: Double?
    private var navigationViewportPaddingRight:  Double?
    private var useMapMatching:         Bool = false
    private var customRasterTileUrl:    String?
    private var customRasterAboveLayerId: String?
    // Whether NavigationViewController's built-in end-of-route screen
    // ("You have arrived" + trip rating stars) is shown on final arrival.
    // Defaults to true = the SDK's own default behavior, so adding this
    // prop changes nothing for existing consumers unless they opt out.
    private var showEndOfRouteFeedback: Bool = true
    // Whether the drop-in's bottom banner (ETA/duration/distance bar +
    // cancel button) is shown (5.0.3 - previously an Android-only prop,
    // dead on iOS). false swaps in an empty bottom banner at
    // NavigationViewController construction (see ExpoEmptyBottomBanner).
    // Construction-time configuration, like the maneuver banner colors:
    // changing it mid-navigation applies on the NEXT route presentation.
    private var showEta: Bool = true

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

    // MARK: - Shared navigation provider (5.0.1)
    // Mapbox Navigation SDK v3 enforces ONE MapboxNavigationProvider per
    // process: its internal checkInstanceIsUnique() deliberately traps
    // (EXC_BREAKPOINT/SIGTRAP) when a second instance is created while
    // another is still alive - confirmed by a real production crash log
    // whose main-thread stack is exactly: closure #1 in static
    // MapboxNavigationProvider.checkInstanceIsUnique() <- Locked.withLock
    // <- MapboxNavigationProvider.init(coreConfig:) <- this file's init
    // block. The previous per-view-instance provider therefore crashed the
    // app whenever the navigation screen was closed and reopened before
    // the previous view instance fully deallocated (an intermittent,
    // remount-timing-dependent crash). All view instances now share this
    // single lazily-created provider - the same pattern Mapbox's own v3
    // examples use (a static/shared provider), and the reason reference
    // implementations of this package keep theirs in a `static let`.
    //
    // This also replaces the 4.0.8-era DispatchQueue.main.async deferral
    // entirely: the shared provider is available synchronously from the
    // first access, so `mapboxNavigation` is set before any prop setter
    // can run - the startup race that caused the permanent black screen
    // is now impossible by construction, and the provider-ready catch-up
    // call is no longer needed (the coalesced prop-setter scheduler from
    // 5.0.0 is the single remaining fetch trigger).
    private static let sharedNavigationProvider = MapboxNavigationProvider(coreConfig: .init())

    // MARK: - Init
    // Route loading overlay (5.1.4): opt-in full-view cover (opaque
    // background + spinner) shown until the first route is presented,
    // hiding the "map appears first, route pops in later" sequence.
    // Opt-in (default false) — zero behavior change for existing
    // consumers. One-shot per view instance: dismissed on first success
    // OR first failure, never re-shown (later refetches happen over an
    // already-presented map). Same contract as Android's implementation.
    private var showRouteLoadingOverlay: Bool = false
    private var loadingOverlayColor:    String?
    private var routeLoadingOverlayView: UIView?
    private var routeLoadingOverlayDone = false

    public required init(appContext: AppContext? = nil) {
        super.init(appContext: appContext)
        let provider = Self.sharedNavigationProvider
        self.mapboxNavigationProvider = provider
        self.mapboxNavigation = provider.mapboxNavigation

        // Built hidden at init — props always arrive AFTER the native view
        // exists (setters below reveal it), and building it up front means
        // it is already in place, above any later-added subview via the
        // bringSubviewToFront call in its setter, before the first route
        // request can possibly resolve. Spinner centered via flexible
        // margins so it tracks the overlay through every resize.
        let overlay = UIView()
        overlay.backgroundColor = mapboxColor(fromHex: "#1E2433")
        overlay.isHidden = true
        overlay.frame = bounds
        overlay.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        let spinner = UIActivityIndicatorView(style: .large)
        spinner.color = .white
        spinner.startAnimating()
        spinner.center = CGPoint(x: overlay.bounds.midX, y: overlay.bounds.midY)
        spinner.autoresizingMask = [
            .flexibleLeftMargin, .flexibleRightMargin,
            .flexibleTopMargin, .flexibleBottomMargin
        ]
        overlay.addSubview(spinner)
        addSubview(overlay)
        routeLoadingOverlayView = overlay
    }

    // Dismisses the route loading overlay permanently (one-shot — see the
    // field declarations above). Fade-out only when it was actually
    // showing; instant no-op when it never was (prop disabled).
    private func finishRouteLoadingOverlay() {
        if routeLoadingOverlayDone { return }
        routeLoadingOverlayDone = true
        guard let ov = routeLoadingOverlayView, !ov.isHidden else { return }
        // The navigation view (or free-drive map) was just added ABOVE this
        // overlay in the subview order — re-front it so the fade-out is
        // actually visible over the freshly presented map instead of the
        // map appearing with a hard cut.
        bringSubviewToFront(ov)
        UIView.animate(withDuration: 0.25, animations: { ov.alpha = 0 }) { _ in
            ov.isHidden = true
            ov.alpha = 1
        }
    }

    func setShowRouteLoadingOverlay(_ v: Bool?) {
        let resolved = v ?? false
        guard resolved != showRouteLoadingOverlay else { return }
        showRouteLoadingOverlay = resolved
        guard let ov = routeLoadingOverlayView else { return }
        if resolved && !routeLoadingOverlayDone {
            ov.isHidden = false
            bringSubviewToFront(ov)
        } else if !resolved {
            ov.isHidden = true
        }
    }

    func setLoadingOverlayColor(_ c: String?) {
        guard c != loadingOverlayColor else { return }
        loadingOverlayColor = c
        routeLoadingOverlayView?.backgroundColor =
            c.flatMap { mapboxColor(fromHex: $0) } ?? mapboxColor(fromHex: "#1E2433")
    }

    // UIKit stacks subviews purely by add-order (no Android-style elevation
    // concept) — the overlay is added first (at init), so both the
    // free-drive fallback map (startFreeDriveFallback) and the real
    // NavigationViewController's view (presentNavigationViewController) get
    // added ABOVE it later and would otherwise cover it while still
    // visible. Called right after each of those addSubview calls.
    private func keepLoadingOverlayOnTop() {
        guard let ov = routeLoadingOverlayView, !ov.isHidden else { return }
        bringSubviewToFront(ov)
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

    // Degenerate-route guard threshold (5.0.1): when EVERY coordinate of a
    // requested trip lies within this radius of the first one, the whole
    // trip is a single point - there is nothing to navigate. A real device
    // test showed what happens otherwise: a 0 m route, instant arrival, and
    // a camera left idle over a default world region. Deliberately checks
    // ALL coordinates against the first (not just origin vs destination):
    // a legitimate round trip (start -> distant via point -> back to start)
    // also has origin == destination and must NOT be blocked.
    // Same threshold and same failure message on Android - kept in parity.
    private static let degenerateRouteThresholdMeters: CLLocationDistance = 25.0
    static let degenerateRouteMessage =
        "Origin and destination are the same location (all waypoints within 25 m) - nothing to navigate"

    // MARK: - Route fetching (parity with Android fetchRoutes)
    private func fetchRoutes() {
        guard coordinates.count >= 2, let mapboxNavigation = mapboxNavigation else { return }

        // Degenerate-route guard (see the threshold constant above): emit a
        // clear, catchable onRoutesFailed instead of starting a 1-second
        // navigation session into an instant arrival screen. The consuming
        // app can present its own "you are already at your destination"
        // message from this event.
        let firstLocation = CLLocation(
            latitude:  coordinates[0]["latitude"]  ?? 0.0,
            longitude: coordinates[0]["longitude"] ?? 0.0
        )
        let wholeTripIsOnePoint = coordinates.allSatisfy { coord in
            let location = CLLocation(
                latitude:  coord["latitude"]  ?? 0.0,
                longitude: coord["longitude"] ?? 0.0
            )
            return location.distance(from: firstLocation) < Self.degenerateRouteThresholdMeters
        }
        if wholeTripIsOnePoint {
            // Show a live free-drive map instead of a black view (5.0.2):
            // without this, no NavigationViewController is ever constructed
            // in this branch, and iOS's route-only architecture would leave
            // the surface permanently empty.
            startFreeDriveFallback()
            finishRouteLoadingOverlay()
            // `code` is a stable, machine-readable identifier: the consuming
            // app should switch on it to show its own LOCALIZED message
            // (the `message` string is English-only debugging text - this
            // package deliberately does not attempt to translate it; the
            // `language` prop only localizes what the Mapbox API generates,
            // never this package's own strings).
            onRoutesFailed([
                "message": Self.degenerateRouteMessage,
                "code": "same_location"
            ])
            return
        }

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

        let resolvedProfile: ProfileIdentifier
        switch navigationProfile ?? "driving-traffic" {
        case "driving-traffic": resolvedProfile = .automobileAvoidingTraffic
        case "driving":         resolvedProfile = .automobile
        case "walking":         resolvedProfile = .walking
        case "cycling":         resolvedProfile = .cycling
        default:                resolvedProfile = .automobileAvoidingTraffic
        }
        // COMPILE FIX (5.0.6): a plain Bool, NOT an explicitly-typed
        // `MeasurementSystem` local. The type itself isn't in scope here
        // (its module isn't imported by this file) - the pre-5.0.5 code
        // only ever used it through inference on the
        // `distanceMeasurementSystem` property, which is also how both
        // branches below assign it. Caught by the first real iOS compile
        // of this refactor (EAS build - this package has no local Xcode).
        let useImperialUnits = resolveVoiceUnits() == "imperial"

        // useMapMatching (5.0.4, zero-dead-props phase - previously
        // stored-only on iOS): routes the request through the Map Matching
        // API instead of Directions. RoutingProvider declares BOTH
        // overloads at v3.20.1 (verified verbatim:
        // `calculateRoutes(options: RouteOptions) -> FetchTask` and
        // `calculateRoutes(options: MatchOptions) -> FetchTask`, same
        // return type), and NavigationMatchOptions is the SDK's own
        // navigation-optimized MatchOptions subclass - so the entire
        // downstream result handling is IDENTICAL for both branches.
        let request: Task<NavigationRoutes, Error>
        if useMapMatching {
            let options = NavigationMatchOptions(waypoints: waypoints, profileIdentifier: resolvedProfile)
            if let langTag = language { options.locale = Locale(identifier: langTag) }
            options.distanceMeasurementSystem = useImperialUnits ? .imperial : .metric
            // NOTE: excludeTypes/maxHeight/maxWidth are Directions-API-only
            // request parameters (RouteOptions properties with no
            // MatchOptions equivalent) - the Map Matching API itself has no
            // such filters. Not dead code: an API-surface difference,
            // documented in the Props table.
            request = mapboxNavigation.routingProvider().calculateRoutes(options: options)
        } else {
            let options = NavigationRouteOptions(waypoints: waypoints, profileIdentifier: resolvedProfile)
            if let langTag = language { options.locale = Locale(identifier: langTag) }
            options.distanceMeasurementSystem = useImperialUnits ? .imperial : .metric
            // RoadClasses(descriptions:) is a FAILABLE initializer (verified
            // verbatim at v3.20.1): any single unrecognized description
            // makes it return nil -> exclusions skipped gracefully, never a
            // crash. Recognized values: "toll", "restricted", "motorway",
            // "ferry", "tunnel", "hov2", "hov3", "hot", "unpaved",
            // "cash_only_tolls".
            if let excludes = excludeTypes, !excludes.isEmpty,
               let roadClasses = RoadClasses(descriptions: excludes) {
                options.roadClassesToAvoid = roadClasses
            }
            // maximumHeight/maximumWidth are Measurement<UnitLength>
            // (verified at v3.20.1); Android's plain numeric values are
            // already meters, so no conversion needed.
            if let h = maxHeight {
                options.maximumHeight = Measurement(value: h, unit: UnitLength.meters)
            }
            if let w = maxWidth {
                options.maximumWidth = Measurement(value: w, unit: UnitLength.meters)
            }
            request = mapboxNavigation.routingProvider().calculateRoutes(options: options)
        }

        routeRequestTask?.cancel()
        routeRequestTask = Task { [weak self] in
            guard let self = self else { return }
            let result = await request.result
            // Swift Task cancellation is cooperative: cancel() only sets a
            // flag, it does not interrupt this body. Without this check, a
            // request superseded by newer coordinates (or cancelled by
            // removeFromSuperview) would still fire stale events and present
            // a NavigationViewController for an outdated route.
            guard !Task.isCancelled else { return }
            switch result {
            case .failure(let error):
                // UIView work — must run on the main actor (the success
                // branch's presentation already does the same).
                await MainActor.run { self.finishRouteLoadingOverlay() }
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

    // MARK: - Waypoint markers (5.0.2)
    // A professional destination marker (checkered "finish" flag on a pole)
    // at the final waypoint, and numbered circular badges at intermediate
    // waypoints - rendered through the SDK's OWN waypoint pipeline via the
    // three NavigationViewControllerDelegate customization hooks
    // (shapeFor/waypointCircleLayer/waypointSymbolLayer), all verified
    // verbatim against Mapbox's own v3.20.1 Custom-Final-Waypoint example.
    // Using the SDK pipeline (rather than manually managed annotations)
    // means markers automatically follow rerouting, and passed waypoints
    // fade out via the same `completed` property the SDK maintains.

    static let finalWaypointImageId = "expo_mapbox_navigation_final_flag"

    // Draws the checkered destination flag (pole + 3x2 checker) in code -
    // no bundled image assets, consistent with how every icon in this
    // package's Android UI is drawn. Size in points; rendered at screen
    // scale by UIGraphicsImageRenderer automatically.
    //
    // GEOMETRY CONTRACT (5.1.0 offset fix): the pole's base must sit
    // EXACTLY at the image's bottom-CENTER pixel, because
    // `iconAnchor = .bottom` (see waypointSymbolLayerWithIdentifier below)
    // anchors the image's bottom-center on the map point - the same
    // contract Android's drawDestinationFlagBitmap() already documents and
    // relies on (fixed there in 5.0.3). The PREVIOUS version of this
    // function had the pole at the LEFT edge (x: 2, not centered on a
    // 30pt-wide image) with a 2pt empty margin below it - the exact same
    // class of offset bug 5.0.3 fixed on Android, just never verified here
    // at the time (the 5.0.3 changelog's "iOS needs no change" claim was
    // only about the COORDINATE SOURCE - confirmed correct, traced to
    // `Route.waypointsMapFeature` in NavigationMapStyleManager.swift at
    // v3.20.1, response-derived, not raw request coordinates - it never
    // examined this image's own internal drawing geometry). Pole is now
    // horizontally centered and touches the bottom edge exactly, matching
    // Android's fix pixel-for-pixel in spirit.
    //
    // SIZE (5.1.0, "more prominent" per real-device feedback the flag was
    // too small next to the route line): scaled up from the previous
    // 30x40 to 48x56 - proportionally larger, same design (pole + 3x2
    // checkered banner), matching Android's own proportional enlargement
    // in this same version.
    private static func makeDestinationFlagImage() -> UIImage {
        let size = CGSize(width: 48, height: 56)
        let renderer = UIGraphicsImageRenderer(size: size)
        return renderer.image { ctx in
            let c = ctx.cgContext
            let poleCenterX = size.width / 2
            // Pole: rounded vertical bar, horizontally centered, base
            // flush with the image's bottom edge (the anchor point).
            let poleRect = CGRect(x: poleCenterX - 2, y: 3, width: 4, height: size.height - 3)
            c.setFillColor(UIColor(red: 0.18, green: 0.23, blue: 0.29, alpha: 1).cgColor)
            UIBezierPath(roundedRect: poleRect, cornerRadius: 2).fill()
            // Flag body: rectangle attached to the pole top, extending right.
            let flagRect = CGRect(x: poleCenterX + 2, y: 4, width: 20, height: 18)
            // White base + thin dark border, then 3x2 checker overlay.
            c.setFillColor(UIColor.white.cgColor)
            c.fill(flagRect)
            let cellW = flagRect.width / 3.0
            let cellH = flagRect.height / 2.0
            c.setFillColor(UIColor(red: 0.12, green: 0.14, blue: 0.17, alpha: 1).cgColor)
            for row in 0..<2 {
                for col in 0..<3 where (row + col) % 2 == 0 {
                    c.fill(CGRect(
                        x: flagRect.minX + CGFloat(col) * cellW,
                        y: flagRect.minY + CGFloat(row) * cellH,
                        width: cellW,
                        height: cellH
                    ))
                }
            }
            c.setStrokeColor(UIColor(red: 0.12, green: 0.14, blue: 0.17, alpha: 1).cgColor)
            c.setLineWidth(1)
            c.stroke(flagRect)
        }
    }

    // Registers the flag image with the given map's style if not already
    // present. Inlined equivalent of the v3.20.1 example's own
    // addImageIfNotExists helper (imageExists(withId:) + addImage(_:id:...)
    // are the public MapboxMaps APIs it wraps) - with try? instead of the
    // example's try!, per this package's never-crash-on-style-ops rule.
    private func registerFlagImage(on navigationMapView: NavigationMapView) {
        guard !navigationMapView.mapView.mapboxMap.imageExists(withId: Self.finalWaypointImageId) else { return }
        try? navigationMapView.mapView.mapboxMap.addImage(
            Self.makeDestinationFlagImage(),
            id: Self.finalWaypointImageId,
            stretchX: [],
            stretchY: []
        )
    }

    // customRasterTileUrl/customRasterAboveLayerId (5.0.4, zero-dead-props
    // phase - previously stored-only on BOTH platforms since before 4.0.2).
    // Injects a raster tile source + layer into the drop-in map's style.
    // API pattern (RasterSource(id:)/tiles/tileSize, RasterLayer(id:source:),
    // addSource/addLayer(layerPosition: .above)) verified against a real
    // same-SDK-generation implementation compiled in production. Idempotent:
    // removes any previous instance first, so it is safe to call on every
    // style load AND on live prop changes; a nil URL simply cleans up.
    // Style operations use try? - never a crash source.
    private static let customRasterSourceId = "expo_mapbox_navigation_raster_source"
    private static let customRasterLayerId = "expo_mapbox_navigation_raster_layer"

    private func applyCustomRasterLayer(on navigationMapView: NavigationMapView) {
        // COMPILE FIX (5.0.6): guard-let, not a plain `let` binding.
        // MapView.mapboxMap is an implicitly-unwrapped optional
        // (`MapboxMap!`): direct call chains auto-unwrap it (which is why
        // registerFlagImage's chained calls have compiled since 5.0.3),
        // but binding it to a local demotes it to an ordinary `MapboxMap?`
        // whose members then require unwrapping - the exact EAS compile
        // error this fixes. guard-let also gives the nil case a graceful
        // no-op, per this package's never-crash-on-style-ops rule.
        guard let mapboxMap = navigationMapView.mapView.mapboxMap else { return }
        if mapboxMap.layerExists(withId: Self.customRasterLayerId) {
            try? mapboxMap.removeLayer(withId: Self.customRasterLayerId)
        }
        if mapboxMap.sourceExists(withId: Self.customRasterSourceId) {
            try? mapboxMap.removeSource(withId: Self.customRasterSourceId)
        }
        guard let tileUrl = customRasterTileUrl, !tileUrl.isEmpty else { return }
        var rasterSource = RasterSource(id: Self.customRasterSourceId)
        rasterSource.tiles = [tileUrl]
        rasterSource.tileSize = 256
        let rasterLayer = RasterLayer(id: Self.customRasterLayerId, source: Self.customRasterSourceId)
        try? mapboxMap.addSource(rasterSource)
        if let aboveLayerId = customRasterAboveLayerId, !aboveLayerId.isEmpty {
            try? mapboxMap.addLayer(rasterLayer, layerPosition: .above(aboveLayerId))
        } else {
            try? mapboxMap.addLayer(rasterLayer)
        }
    }

    // MARK: - Free-drive fallback map (5.0.2)
    // Construction pattern verified VERBATIM against Mapbox's own v3.20.1
    // example (Examples/AdditionalExamples/Examples/
    // Custom-Navigation-Camera.swift): a standalone NavigationMapView built
    // from the shared navigation's Combine publishers
    // (locationMatching.map(\.enhancedLocation) /
    // routeProgress.map(\.?.routeProgress)), plus
    // tripSession().startFreeDrive() - all three APIs confirmed public at
    // the exact vendored tag (SessionController.swift line 8,
    // NavigationMapView.swift line 113, NavigationController.swift).
    // The default puck on a NavigationMapView is already the SDK's
    // directional arrow (.puck3D(.navigationDefault)); custom puck props
    // deliberately do not apply here - this is a minimal passive fallback,
    // not a navigation session.
    private func startFreeDriveFallback() {
        guard let mapboxNavigation = mapboxNavigation else { return }
        // Already showing: just re-assert the following camera.
        if let existing = freeDriveMapView {
            existing.navigationCamera.update(cameraState: .following)
            return
        }
        tearDownNavigationViewController()

        let mapView = NavigationMapView(
            location: mapboxNavigation.navigation().locationMatching.map(\.enhancedLocation)
                .eraseToAnyPublisher(),
            routeProgress: mapboxNavigation.navigation().routeProgress.map(\.?.routeProgress)
                .eraseToAnyPublisher()
        )
        addSubview(mapView)
        mapView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            mapView.topAnchor.constraint(equalTo: topAnchor),
            mapView.bottomAnchor.constraint(equalTo: bottomAnchor),
            mapView.leadingAnchor.constraint(equalTo: leadingAnchor),
            mapView.trailingAnchor.constraint(equalTo: trailingAnchor)
        ])

        mapboxNavigation.tripSession().startFreeDrive()
        mapView.navigationCamera.update(cameraState: .following)
        freeDriveMapView = mapView
        keepLoadingOverlayOnTop()
    }

    private func tearDownFreeDriveMapView() {
        freeDriveMapView?.removeFromSuperview()
        freeDriveMapView = nil
    }

    // MARK: - Present NavigationViewController (drop-in: includes ETA bar,
    // speed limit, lane guidance, mute/overview/recenter buttons natively)
    private func presentNavigationViewController(with navigationRoutes: NavigationRoutes) {
        guard let provider = mapboxNavigationProvider,
              let mapboxNavigation = mapboxNavigation else { return }

        // A real route replaces the free-drive fallback map, if one is up
        // (the SDK's own examples do this exact free-drive -> active
        // guidance transition; NavigationViewController manages the session
        // switch itself).
        tearDownFreeDriveMapView()
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
        // Custom colors (5.0.4, zero-dead-props phase) - previously
        // stored-only on iOS, now applied through the same construction-time
        // style mechanism as the maneuver background (see
        // applyExpoCustomColors for the verified appearance targets).
        let turnIcon = maneuverTurnIconColor.flatMap { mapboxColor(fromHex: $0) }
        let etaBg = etaBarBackgroundColor.flatMap { mapboxColor(fromHex: $0) }
        let etaText = etaTextColor.flatMap { mapboxColor(fromHex: $0) }
        let iconBtn = iconButtonColor.flatMap { mapboxColor(fromHex: $0) }
        for style in [dayStyle as Any, nightStyle as Any] {
            if let day = style as? ExpoManeuverDayStyle {
                day.turnIconColor = turnIcon
                day.etaBarBackgroundColor = etaBg
                day.etaTextColor = etaText
                day.iconButtonColor = iconBtn
            } else if let night = style as? ExpoManeuverNightStyle {
                night.turnIconColor = turnIcon
                night.etaBarBackgroundColor = etaBg
                night.etaTextColor = etaText
                night.iconButtonColor = iconBtn
            }
        }

        let navigationOptions = NavigationOptions(
            mapboxNavigation: mapboxNavigation,
            voiceController:  provider.routeVoiceController,
            eventsManager:    provider.eventsManager(),
            styles: [dayStyle, nightStyle],
            // showEta=false (5.0.3): replace the default bottom banner
            // (ETA bar + cancel button) with an empty one - the SDK's own
            // customization point, no show/hide timing races. nil keeps
            // the SDK default exactly as before.
            bottomBanner: showEta ? nil : ExpoEmptyBottomBanner()
        )

        let vc = NavigationViewController(
            navigationRoutes:  navigationRoutes,
            navigationOptions: navigationOptions
        )
        vc.delegate = self
        vc.routeLineTracksTraversal = true
        // Built-in end-of-route feedback screen toggle. Verified verbatim at
        // v3.20.1 (NavigationViewController.swift line 522):
        // `public var showsEndOfRouteFeedback: Bool` with a real setter
        // (forwards to arrivalController.showsEndOfRoute). NOTE, from the
        // same source: assigning `showsReportFeedback` OVERWRITES this flag
        // in its didSet - this package never assigns showsReportFeedback,
        // so the value set here sticks.
        vc.showsEndOfRouteFeedback = showEndOfRouteFeedback

        // Apply mute state
        if mute {
            provider.routeVoiceController.speechSynthesizer.muted = true
        }

        // View-controller containment, in Apple's documented canonical
        // order ("Creating a custom container view controller"):
        // addChild FIRST, then add the child's view / constraints, then
        // didMove(toParent:) last. The previous ordering here (addSubview
        // before addChild) worked but violated that documented contract,
        // which can skew appearance-callback ordering. Containment is
        // still conditional on actually finding a parent VC — same
        // fallback behavior as before when none is reachable.
        let parentVC = findParentViewController()
        if let parentVC = parentVC {
            parentVC.addChild(vc)
        }
        addSubview(vc.view)
        vc.view.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            vc.view.topAnchor.constraint(equalTo: topAnchor),
            vc.view.bottomAnchor.constraint(equalTo: bottomAnchor),
            vc.view.leadingAnchor.constraint(equalTo: leadingAnchor),
            vc.view.trailingAnchor.constraint(equalTo: trailingAnchor)
        ])
        if parentVC != nil {
            vc.didMove(toParent: parentVC)
        }
        keepLoadingOverlayOnTop()

        // AUTOMATIC DAY/NIGHT FIX (5.1.0): NavigationViewController.viewDidLoad()
        // (triggered above by `addSubview(vc.view)`, which forces the view to
        // load) calls its own internal setupNavigation(), which - verified
        // verbatim at v3.20.1 - runs setupStyleManager(navigationOptions)
        // BEFORE mapboxNavigation.tripSession().startActiveGuidance(...). At
        // the exact moment StyleManager first computes sunrise/sunset for the
        // day/night switch, its own StyleManagerDelegate.location(for:)
        // implementation (also the SDK's own, on NavigationViewController)
        // reads `route`, a computed property backed by
        // `tripSession().currentNavigationRoutes` - which is still nil,
        // since the trip session hasn't started yet. Location resolution
        // fails, StyleManager silently falls back to `styles.first` (day,
        // given how `styles: [dayStyle, nightStyle]` is ordered above) -
        // permanently: `resetTimeOfDayTimer()` fails the same way at the
        // same moment, so no timer is ever scheduled to retry later either.
        // Net effect: the map never shows night style automatically, no
        // matter the actual time - a real device report confirmed this
        // (map stayed light at night, every time, not intermittently).
        //
        // Fixed using only the SDK's own public API, not by reimplementing
        // sunrise/sunset math: `StyleManager.styles`'s public setter has a
        // `didSet` that internally calls BOTH `applyStyle()` (re-evaluates
        // and applies the correct style for the CURRENT time) AND
        // `resetTimeOfDayTimer()` (schedules the next correct transition) -
        // confirmed verbatim in StyleManager.swift. Re-assigning the same
        // array to itself triggers that didSet. By this point `startActive
        // Guidance` has already run (it's the line right after
        // setupStyleManager() inside the same synchronous setupNavigation()
        // call, itself already complete since accessing `vc.view` above
        // forced viewDidLoad to run to completion) - so `route` now resolves
        // and the sunrise/sunset calculation succeeds.
        vc.styleManager.styles = vc.styleManager.styles

        // Tap on banner → emit full steps list (parity with Android banner tap)
        attachManeuverBannerTapHandler(to: vc)

        self.navigationViewController = vc
        isOverviewMode = false

        // Destination flag image + custom raster layer: apply now, and
        // re-apply after every style load (style changes wipe both
        // registered images AND injected sources/layers - including this
        // package's own optional mapStyle load below). Same double coverage
        // as Mapbox's own v3.20.1 Custom-Final-Waypoint example.
        registerFlagImage(on: vc.navigationView.navigationMapView)
        applyCustomRasterLayer(on: vc.navigationView.navigationMapView)
        waypointImageStyleCancelable = vc.navigationView.navigationMapView.mapView.mapboxMap
            .onStyleLoaded.observe { [weak self, weak vc] _ in
                guard let self = self, let vc = vc else { return }
                self.registerFlagImage(on: vc.navigationView.navigationMapView)
                self.applyCustomRasterLayer(on: vc.navigationView.navigationMapView)
            }
        // Initial mute-button tint (applies iconButtonMutedColor if the
        // view mounted already muted).
        updateMuteButtonTint()

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
        applyViewportPadding(to: vc.navigationView.navigationMapView)

        // Force the camera into follow-the-driver mode immediately on
        // presentation. Verified verbatim at v3.20.1:
        // NavigationCamera.update(cameraState:) is public
        // (Map/Camera/NavigationCamera.swift line 114) and
        // NavigationCameraState.following is "the camera is following user
        // position". Normally active guidance enters this state on its own,
        // but a real device test surfaced a case where it never did: a
        // degenerate route whose destination equals the driver's current
        // position (0 m, instant arrival) left the camera idle over a
        // default world region (mid-ocean) instead of the driver. Setting
        // .following explicitly here guarantees the map starts centered and
        // zoomed on the driver in every case, including that one - the same
        // call the reference implementation uses for its own recenter
        // button, so it is also harmless in the normal (non-degenerate)
        // flow where the SDK would have entered following anyway.
        vc.navigationView.navigationMapView.navigationCamera.update(cameraState: .following)

        // Route presented and camera positioned — dismiss the loading
        // overlay (one-shot; no-op if it was never shown/enabled).
        finishRouteLoadingOverlay()
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
        updateMuteButtonTint()
    }

    // iconButtonMutedColor (5.0.4, zero-dead-props phase): tints the drop-in's
    // mute floating button when voice is muted. `floatingButtons` is a
    // public [UIButton]? on NavigationViewController (verified at v3.20.1,
    // line 643), and the SDK's own doc comment on it states the default
    // buttons are "the overview, mute and feedback report button" - so the
    // mute button is index 1. That ordering is part of the SDK we vendor at
    // a FIXED version, and the access is bounds-checked: if the array is
    // shorter (custom configurations), this is a silent no-op, never a
    // crash.
    private func updateMuteButtonTint() {
        guard let buttons = navigationViewController?.floatingButtons, buttons.count > 1 else { return }
        let normal = iconButtonColor.flatMap { mapboxColor(fromHex: $0) }
        let muted = iconButtonMutedColor.flatMap { mapboxColor(fromHex: $0) }
        if isMuted {
            if let muted = muted { buttons[1].tintColor = muted }
        } else if let normal = normal {
            buttons[1].tintColor = normal
        }
    }

    // MARK: - Cancel (parity with Android cancelNavigation)
    private func cancelNavigation() {
        tearDownNavigationViewController()
        onNavigationCancelled([:])
    }

    // MARK: - Teardown
    private func tearDownNavigationViewController() {
        waypointImageStyleCancelable?.cancel()
        waypointImageStyleCancelable = nil
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
    //
    // ── Coalesced route refetching (5.0.0) ──────────────────────────────────
    // Every prop that fetchRoutes() actually reads now re-triggers a route
    // calculation when its value CHANGES (previously only `coordinates` did;
    // changing e.g. navigationProfile or language after mount was silently
    // ignored until the next coordinates change — stale-language voice
    // guidance was a concrete consequence). Implemented identically on
    // Android in the same release — cross-platform parity preserved. Two
    // safeguards against request storms / regressions:
    //   1. Equality guard in each setter — no refetch when RN re-delivers an
    //      identical value.
    //   2. Coalescing — triggers within the same main-runloop turn merge
    //      into ONE deferred fetchRoutes() call (initial mount delivers all
    //      props in a single batch; without this, up to 8 simultaneous
    //      route requests would fire). Deferring also means the initial
    //      batch's single fetch runs AFTER init()'s provider-creation block
    //      (queued on the main queue first), closing the startup ordering
    //      gap from yet another angle on top of 4.0.8's catch-up call.
    // fetchRoutes() itself keeps its own guards (mapboxNavigation != nil,
    // coordinates.count >= 2), so a scheduled fetch with no valid
    // coordinates is a harmless no-op. Props NOT read by fetchRoutes
    // (mapStyle, mute, useMapMatching, customRaster*, all color props)
    // deliberately do NOT schedule a refetch.
    private var fetchScheduled = false
    private func scheduleFetchRoutes() {
        guard !fetchScheduled else { return }
        fetchScheduled = true
        DispatchQueue.main.async { [weak self] in
            guard let self = self else { return }
            self.fetchScheduled = false
            self.fetchRoutes()
        }
    }

    func setCoordinates(_ coords: [[String: Double]]) {
        guard coords != coordinates else { return }
        coordinates = coords
        scheduleFetchRoutes()
    }
    func setWaypointIndices(_ i: [Int]?) {
        guard i != waypointIndices else { return }
        waypointIndices = i
        scheduleFetchRoutes()
    }
    func setLanguage(_ l: String?) {
        guard l != language else { return }
        language = l
        scheduleFetchRoutes()
    }
    func setVoiceUnits(_ u: String?) {
        guard u != voiceUnits else { return }
        voiceUnits = u
        scheduleFetchRoutes()
    }
    func setNavigationProfile(_ p: String?) {
        guard p != navigationProfile else { return }
        navigationProfile = p
        scheduleFetchRoutes()
    }
    func setExcludeTypes(_ t: [String]?) {
        guard t != excludeTypes else { return }
        excludeTypes = t
        scheduleFetchRoutes()
    }
    func setMapStyle(_ s: String?)         { mapStyle = s }
    func setMute(_ m: Bool)                { mute = m; applyMute(m) }
    // Applied at NavigationViewController construction, and also live on an
    // already-presented controller (the SDK property has a real setter, so
    // toggling mid-navigation works). Not a route-affecting prop - no
    // refetch scheduled.
    func setShowEndOfRouteFeedback(_ s: Bool) {
        showEndOfRouteFeedback = s
        navigationViewController?.showsEndOfRouteFeedback = s
    }
    // iOS implementation added in 5.0.3 (previously Android-only, dead
    // here). Construction-time: applies when the next route presents its
    // NavigationViewController - not a refetch trigger.
    func setShowEta(_ s: Bool) {
        showEta = s
    }
    func setMaxHeight(_ h: Double?) {
        guard h != maxHeight else { return }
        maxHeight = h
        scheduleFetchRoutes()
    }
    // Navigation icon position overrides (5.1.0) - camera framing only,
    // never affects the route, so these apply live to the already-presented
    // navigationMapView instead of scheduling a refetch (same pattern as
    // setCustomRasterTileUrl below).
    func setNavigationViewportPaddingTop(_ v: Double?) {
        guard v != navigationViewportPaddingTop else { return }
        navigationViewportPaddingTop = v
        if let vc = navigationViewController { applyViewportPadding(to: vc.navigationView.navigationMapView) }
    }
    func setNavigationViewportPaddingLeft(_ v: Double?) {
        guard v != navigationViewportPaddingLeft else { return }
        navigationViewportPaddingLeft = v
        if let vc = navigationViewController { applyViewportPadding(to: vc.navigationView.navigationMapView) }
    }
    func setNavigationViewportPaddingBottom(_ v: Double?) {
        guard v != navigationViewportPaddingBottom else { return }
        navigationViewportPaddingBottom = v
        if let vc = navigationViewController { applyViewportPadding(to: vc.navigationView.navigationMapView) }
    }
    func setNavigationViewportPaddingRight(_ v: Double?) {
        guard v != navigationViewportPaddingRight else { return }
        navigationViewportPaddingRight = v
        if let vc = navigationViewController { applyViewportPadding(to: vc.navigationView.navigationMapView) }
    }
    func setMaxWidth(_ w: Double?) {
        guard w != maxWidth else { return }
        maxWidth = w
        scheduleFetchRoutes()
    }
    // useMapMatching switches which API the route request uses (5.0.4) -
    // it IS a route-affecting prop now, so changes re-trigger the fetch.
    func setUseMapMatching(_ u: Bool) {
        guard u != useMapMatching else { return }
        useMapMatching = u
        scheduleFetchRoutes()
    }
    // Raster overlay props (5.0.4): applied live on the presented map when
    // one exists; always re-applied on style loads via the style observer.
    func setCustomRasterTileUrl(_ u: String?) {
        guard u != customRasterTileUrl else { return }
        customRasterTileUrl = u
        if let vc = navigationViewController {
            applyCustomRasterLayer(on: vc.navigationView.navigationMapView)
        }
    }
    func setCustomRasterAboveLayerId(_ l: String?) {
        guard l != customRasterAboveLayerId else { return }
        customRasterAboveLayerId = l
        if let vc = navigationViewController {
            applyCustomRasterLayer(on: vc.navigationView.navigationMapView)
        }
    }

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

        // Default (no custom puck prop set): the SDK's own navigation puck -
        // a 3D directional arrow. Verified verbatim at mapbox-navigation-ios
        // v3.20.1: NavigationMapView.puckType's own default IS
        // .puck3D(.navigationDefault) (NavigationMapView.swift line 348),
        // and Puck2DConfiguration/Puck3DConfiguration.navigationDefault are
        // public statics defined in Map/Other/PuckConfigurations.swift.
        // The previous fallback here (.puck2D(Puck2DConfiguration())) was a
        // real bug: it actively REPLACED the SDK's directional-arrow default
        // with the plain blue location dot, so active navigation showed a
        // dot with no heading indication. Setting the SDK default explicitly
        // (rather than skipping the assignment) also correctly RESTORES the
        // arrow when a custom puck prop is later cleared back to nil.
        mapView.puckType = .puck3D(.navigationDefault)
    }

    // Navigation icon position (5.1.0): NavigationMapView.viewportPadding is
    // the SDK's own public property for this (verified verbatim at v3.20.1,
    // NavigationMapView.swift - "The padding applied to the viewport in
    // addition to the safe area", default
    // UIEdgeInsets(top: 20, left: 20, bottom: 40, right: 20)). Applying all
    // four values unconditionally (falling back to that same SDK default
    // per-side when a prop is nil) rather than only setting it when at
    // least one override is present, so a later prop CHANGE back to nil
    // correctly restores the SDK default too - not just the initial value.
    private func applyViewportPadding(to navigationMapView: NavigationMapView) {
        // COMPILE FIX (audited before publish): UIEdgeInsets' initializer
        // expects CGFloat, but `navigationViewportPaddingTop ?? 20` (etc.)
        // resolves to Double - `??`'s both sides unify against the Double?
        // property's wrapped type BEFORE the outer UIEdgeInsets(top:) call
        // is considered, so the literal `20` is inferred as Double, not
        // CGFloat. Swift does not implicitly convert Double to CGFloat as
        // a function argument (they are distinct concrete types) - this
        // would have failed exactly like the MeasurementSystem/IUO-binding
        // compile errors found in earlier EAS builds this same version
        // range. Explicit CGFloat(...) conversion on each value fixes it.
        navigationMapView.viewportPadding = UIEdgeInsets(
            top: CGFloat(navigationViewportPaddingTop ?? 20),
            left: CGFloat(navigationViewportPaddingLeft ?? 20),
            bottom: CGFloat(navigationViewportPaddingBottom ?? 40),
            right: CGFloat(navigationViewportPaddingRight ?? 20)
        )
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
        // If the free-drive fallback was active, stop its passive session
        // (the provider is shared and process-lived as of 5.0.1 - leaving
        // free drive running after the view is gone would keep location
        // processing alive in the background for nothing). Deliberately
        // scoped to the free-drive case only: active-guidance session
        // teardown stays owned by NavigationViewController exactly as
        // before.
        if freeDriveMapView != nil {
            mapboxNavigation?.tripSession().setToIdle()
            tearDownFreeDriveMapView()
        }
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

    // FIXED signature: this method previously declared `-> Bool` (the
    // Navigation SDK v2 shape of this delegate callback). At v3.20.1 the
    // protocol requirement is `-> Void` (verified verbatim in
    // mapbox-navigation-ios v3.20.1's
    // Sources/MapboxNavigationUIKit/NavigationViewControllerDelegate.swift:
    // both the requirement and its default implementation return Void).
    // With the stray `-> Bool`, this method did NOT match the protocol
    // requirement - it still compiled (just an extra unrelated method on
    // the extension), but the SDK dispatched to its own default no-op
    // implementation instead, so onArrival would NEVER have fired. No
    // compiler diagnostic catches this; found by manually diffing the
    // delegate signatures against the SDK source. Multi-leg continuation
    // is handled by the SDK itself in v3 - the v2-era `return true` had
    // no v3 equivalent to preserve.
    public func navigationViewController(
        _ navigationViewController: NavigationViewController,
        didArriveAt waypoint: Waypoint
    ) {
        onArrival([:])
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

    // MARK: Waypoint marker customization (5.0.2)
    // The three SDK hooks below are adapted from Mapbox's own v3.20.1
    // Custom-Final-Waypoint example (verified verbatim: identical method
    // signatures on NavigationViewControllerDelegate at the vendored tag).
    // Per-feature properties drive everything: the final waypoint renders
    // the checkered flag (imageId -> symbol layer icon), intermediate
    // waypoints render as numbered badges (circle layer disc + symbol
    // layer text), and waypoints the driver has already passed fade out
    // via the SDK-maintained legIndex.

    public func navigationViewController(
        _ navigationViewController: NavigationViewController,
        shapeFor waypoints: [Waypoint],
        legIndex: Int
    ) -> FeatureCollection? {
        var features = [Turf.Feature]()
        for (index, waypoint) in waypoints.enumerated() {
            var feature = Feature(geometry: .point(Point(waypoint.coordinate)))
            let isFinal = index == waypoints.count - 1
            let isCompleted = index <= legIndex
            var properties: [String: JSONValue] = [:]
            properties["completedOrFinal"] = .boolean(isCompleted || isFinal)
            properties["completed"] = .boolean(isCompleted)
            properties["imageId"] = isFinal ? .string(Self.finalWaypointImageId) : nil
            // 1-based badge number for intermediate stops only.
            properties["waypointNumber"] = (!isFinal && index > 0) ? .string(String(index)) : nil
            feature.properties = properties
            features.append(feature)
        }
        return FeatureCollection(features: features)
    }

    public func navigationViewController(
        _ navigationViewController: NavigationViewController,
        waypointCircleLayerWithIdentifier identifier: String,
        sourceIdentifier: String
    ) -> CircleLayer? {
        // Badge disc for intermediate waypoints. Hidden for the final
        // waypoint (the flag replaces it) and for already-passed waypoints.
        var circleLayer = CircleLayer(id: identifier, source: sourceIdentifier)
        let hiddenWhenCompletedOrFinal = Exp(.switchCase) {
            Exp(.any) { Exp(.get) { "completedOrFinal" } }
            0
            1
        }
        circleLayer.circleColor = .constant(StyleColor(UIColor(red: 0.18, green: 0.23, blue: 0.29, alpha: 1)))
        circleLayer.circleStrokeColor = .constant(StyleColor(.white))
        circleLayer.circleRadius = .expression(Exp(.interpolate) {
            Exp(.linear)
            Exp(.zoom)
            10.0; 8.0
            18.0; 13.0
        })
        circleLayer.circleStrokeWidth = .constant(2.0)
        circleLayer.circleOpacity = .expression(hiddenWhenCompletedOrFinal)
        circleLayer.circleStrokeOpacity = .expression(hiddenWhenCompletedOrFinal)
        circleLayer.circleEmissiveStrength = .constant(1)
        circleLayer.circlePitchAlignment = .constant(.map)
        return circleLayer
    }

    public func navigationViewController(
        _ navigationViewController: NavigationViewController,
        waypointSymbolLayerWithIdentifier identifier: String,
        sourceIdentifier: String
    ) -> SymbolLayer? {
        // Flag icon at the final waypoint + white badge numbers on the
        // intermediate discs. Features without the matching property render
        // neither (absent imageId -> no icon; absent waypointNumber -> no
        // text), so one layer cleanly serves both roles.
        var symbolLayer = SymbolLayer(id: identifier, source: sourceIdentifier)
        symbolLayer.iconImage = .expression(Exp(.get) { "imageId" })
        symbolLayer.iconAnchor = .constant(.bottom)
        // OFFSET FIX (5.1.0): the previous `iconOffset: [0, 2]` was an
        // ad-hoc, incomplete attempt to compensate for the destination
        // flag image's own pole-not-flush-with-bottom-edge bug (see
        // makeDestinationFlagImage's GEOMETRY CONTRACT comment) - it
        // nudged the icon by a fixed 2pt but never addressed the pole
        // also being off-center horizontally, and didn't fully cancel the
        // vertical gap either. Now that the image itself is drawn
        // correctly (pole centered, base flush with the bottom edge),
        // `iconAnchor: .bottom` alone places it exactly on the waypoint
        // coordinate with no extra offset needed - matching Android, which
        // has no equivalent per-marker offset compensation either.
        symbolLayer.iconAllowOverlap = .constant(true)
        let hiddenWhenCompleted = Exp(.switchCase) {
            Exp(.any) { Exp(.get) { "completed" } }
            0
            1
        }
        symbolLayer.iconOpacity = .expression(hiddenWhenCompleted)
        symbolLayer.textField = .expression(Exp(.get) { "waypointNumber" })
        symbolLayer.textColor = .constant(StyleColor(.white))
        symbolLayer.textSize = .expression(Exp(.interpolate) {
            Exp(.linear)
            Exp(.zoom)
            10.0; 10.0
            18.0; 14.0
        })
        symbolLayer.textAllowOverlap = .constant(true)
        symbolLayer.textOpacity = .expression(hiddenWhenCompleted)
        return symbolLayer
    }
}
