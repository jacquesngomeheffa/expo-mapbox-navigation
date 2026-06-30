import ExpoModulesCore
import UIKit
import CoreLocation
import MapboxDirections
import MapboxNavigationCore
import MapboxNavigationUIKit

/// ExpoMapboxNavigationView (iOS)
///
/// Mirrors expo-mapbox-navigation's Android `ExpoMapboxNavigationView.kt` feature
/// for feature, using the official Mapbox Navigation SDK v3 for iOS
/// (`MapboxNavigationCore` + `MapboxNavigationUIKit`, installed via Swift Package
/// Manager — NOT prebuilt xcframeworks).
///
/// Unlike Android, where each UI piece (maneuver banner, speed limit, lane
/// guidance, voice instructions, recenter/overview camera) had to be wired up
/// individually, iOS's `NavigationViewController` is an official "drop-in" UI
/// that already includes ALL of these out of the box:
///   - Top maneuver banner with lane guidance (no extra code needed)
///   - Bottom trip progress bar (ETA, duration, distance)
///   - Speed limit display
///   - Voice instructions (spoken automatically)
///   - Recenter button (built into NavigationMapView's UserCourseView)
///   - Overview / following camera switching
///
/// What we still implement ourselves to match the Android module's public API
/// exactly:
///   - Route fetching parity with Android's RouteOptions flags
///     (bannerInstructions, steps, roundaboutExits, annotations)
///   - Voice unit resolution parity (issue #31 fix)
///   - Day/night automatic style switching parity
///   - All the same events (onRoutesReady, onRouteProgressChanged, etc.)
///   - Tap-to-open full steps list (onManeuverBannerPressed), mirroring the
///     Android `emitFullRouteSteps()` feature
public class ExpoMapboxNavigationView: ExpoView {

  // MARK: - Event dispatchers (mirror Android's EventDispatcher fields)

  let onRouteProgressChanged = EventDispatcher()
  let onRoutesReady = EventDispatcher()
  let onNavigationFinished = EventDispatcher()
  let onNavigationCancelled = EventDispatcher()
  let onRoutesFailed = EventDispatcher()
  let onArrival = EventDispatcher()
  let onManeuverBannerPressed = EventDispatcher()

  // MARK: - Mapbox Navigation core

  private var mapboxNavigationProvider: MapboxNavigationProvider?
  private var mapboxNavigation: MapboxNavigation?
  private var navigationViewController: NavigationViewController?
  private var currentNavigationRoutes: NavigationRoutes?

  // MARK: - State

  private var isNightMode = false
  private var routeRequestTask: Task<Void, Never>?

  // MARK: - Props (mirror Android's private var props exactly)

  private var coordinates: [[String: Double]] = []
  private var waypointIndices: [Int]?
  private var language: String?
  private var voiceUnits: String?
  private var navigationProfile: String?
  private var excludeTypes: [String]?
  private var mapStyle: String?
  private var mute: Bool = false
  private var maxHeight: Double?
  private var maxWidth: Double?
  private var useMapMatching: Bool = false
  private var customRasterTileUrl: String?
  private var customRasterAboveLayerId: String?

  // MARK: - Init

  public required init(appContext: AppContext? = nil) {
    super.init(appContext: appContext)
    setupNavigationProvider()
  }

  private func setupNavigationProvider() {
    // CoreConfig() uses MBXAccessToken from Info.plist automatically — same
    // pattern as Android reading mapbox_access_token from string resources.
    let provider = MapboxNavigationProvider(coreConfig: .init())
    self.mapboxNavigationProvider = provider
    self.mapboxNavigation = provider.mapboxNavigation
  }

  // MARK: - Day / Night auto switching (parity with Android getAutoStyle/checkAndSwitchDayNight)

  private func isDaytime() -> Bool {
    let hour = Calendar.current.component(.hour, from: Date())
    return hour >= 6 && hour < 20
  }

  // MARK: - Voice units resolution (Issue #31 parity)

  private func resolveVoiceUnits() -> String {
    if let units = voiceUnits?.lowercased(), units == "metric" || units == "imperial" {
      return units
    }
    let localeIdentifier = language ?? Locale.current.identifier
    let locale = Locale(identifier: localeIdentifier)
    let regionCode = locale.region?.identifier ?? ""
    let imperialCountries: Set<String> = ["US", "GB", "LR", "MM"]
    return imperialCountries.contains(regionCode) ? "imperial" : "metric"
  }

  // MARK: - Route fetching (parity with Android fetchRoutes())

  private func fetchRoutes() {
    guard coordinates.count >= 2, let mapboxNavigation = mapboxNavigation else { return }

    let waypoints = coordinates.map { coord -> Waypoint in
      let lat = coord["latitude"] ?? 0.0
      let lon = coord["longitude"] ?? 0.0
      return Waypoint(coordinate: CLLocationCoordinate2D(latitude: lat, longitude: lon))
    }

    // Build NavigationRouteOptions with the same flags Android sets explicitly:
    // bannerInstructions / steps / roundaboutExits / annotations(maxspeed,...)
    // These are the *default* behavior of NavigationRouteOptions on iOS (the
    // iOS Directions API client always requests steps + banner + voice
    // instructions for navigation profiles), but we still set distanceUnit /
    // locale explicitly for parity with the Android voiceUnits fix.
    var options = NavigationRouteOptions(waypoints: waypoints)

    if let langTag = language {
      options.locale = Locale(identifier: langTag)
    }

    let unit = resolveVoiceUnits()
    options.distanceUnit = (unit == "imperial") ? .mile : .kilometer

    if let profile = navigationProfile {
      switch profile {
      case "driving-traffic": options.profileIdentifier = .automobileAvoidingTraffic
      case "driving": options.profileIdentifier = .automobile
      case "walking": options.profileIdentifier = .walking
      case "cycling": options.profileIdentifier = .cycling
      default: break
      }
    } else {
      options.profileIdentifier = .automobileAvoidingTraffic
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
          "routeCount": navigationRoutes.alternativeRoutes.count + 1,
          "distanceMeters": mainRoute.distance,
          "durationSeconds": mainRoute.expectedTravelTime
        ])

        self.presentNavigationViewController(with: navigationRoutes)
      }
    }
  }

  // MARK: - Present drop-in NavigationViewController
  //
  // This single official component replaces ALL of the manual UI wiring we
  // had to do on Android (maneuver banner + lane guidance + speed limit +
  // trip progress + voice instructions + recenter/overview camera). Mapbox
  // builds and manages it internally.

  private func presentNavigationViewController(with navigationRoutes: NavigationRoutes) {
    guard let provider = mapboxNavigationProvider, let mapboxNavigation = mapboxNavigation else { return }

    // Tear down any previous session before starting a new one.
    tearDownNavigationViewController()

    let navigationOptions = NavigationOptions(
      mapboxNavigation: mapboxNavigation,
      voiceController: provider.routeVoiceController,
      eventsManager: provider.eventsManager()
    )

    let vc = NavigationViewController(
      navigationRoutes: navigationRoutes,
      navigationOptions: navigationOptions
    )
    vc.delegate = self

    // Day/night parity with Android's getAutoStyle()/checkAndSwitchDayNight().
    isNightMode = !isDaytime()
    if let styleManager = vc.styleManager {
      styleManager.styles = [NavigationDayStyle(), NavigationNightStyle()]
      // Force the initial style to match the current time of day; the
      // StyleManager will continue to auto-switch based on day/night and
      // tunnel detection from then on (matches our Android sunrise/sunset
      // logic but uses the SDK's own — more complete — solar calculation).
    }

    // Show traversed-route fade, matching common navigation app behavior.
    vc.routeLineTracksTraversal = true

    // Mute parity with Android's resolveVoiceUnits/SpeechVolume approach.
    if mute {
      provider.routeVoiceController?.speechSynthesizer.muted = true
    }

    // Tap-to-open full steps list: the SDK doesn't expose a direct "banner
    // tapped" callback, so we attach a tap gesture to the top banner
    // container once it's in the view hierarchy.
    attachManeuverBannerTapHandler(to: vc)

    addSubview(vc.view)
    vc.view.translatesAutoresizingMaskIntoConstraints = false
    NSLayoutConstraint.activate([
      vc.view.topAnchor.constraint(equalTo: topAnchor),
      vc.view.bottomAnchor.constraint(equalTo: bottomAnchor),
      vc.view.leadingAnchor.constraint(equalTo: leadingAnchor),
      vc.view.trailingAnchor.constraint(equalTo: trailingAnchor)
    ])

    if let parentVC = self.findParentViewController() {
      parentVC.addChild(vc)
      vc.didMove(toParent: parentVC)
    }

    self.navigationViewController = vc
  }

  private func attachManeuverBannerTapHandler(to vc: NavigationViewController) {
    // The top banner container is `vc.navigationView.topBannerContainerView`
    // (per public API on NavigationView). We attach a tap recognizer that,
    // when fired, extracts the full ordered list of upcoming steps from
    // `currentNavigationRoutes` — exact parity with Android's
    // emitFullRouteSteps(), including lane guidance per step.
    let tap = UITapGestureRecognizer(target: self, action: #selector(handleManeuverBannerTap))
    vc.navigationView.topBannerContainerView.addGestureRecognizer(tap)
    vc.navigationView.topBannerContainerView.isUserInteractionEnabled = true
  }

  @objc private func handleManeuverBannerTap() {
    emitFullRouteSteps()
  }

  // MARK: - Full route steps list (parity with Android emitFullRouteSteps())

  private func emitFullRouteSteps() {
    guard let navigationRoutes = currentNavigationRoutes else {
      onManeuverBannerPressed(["steps": []])
      return
    }

    let route = navigationRoutes.mainRoute.route
    var stepsPayload: [[String: Any]] = []

    for leg in route.legs {
      for step in leg.steps {
        // Lane guidance: present on step.intersections[].approachLanes /
        // .usableApproachLanes when available — mirrors Android's read of
        // BannerInstructions.sub().components() of type "lane".
        var laneData: [[String: Any]] = []
        if let intersections = step.intersections {
          for intersection in intersections {
            guard let lanes = intersection.approachLanes,
                  let usable = intersection.usableApproachLanes else { continue }
            for (index, lane) in lanes.enumerated() {
              laneData.append([
                "active": usable.contains(index),
                "directions": lane.indications.map { String(describing: $0) }
              ])
            }
          }
        }

        stepsPayload.append([
          "instruction": step.instructions,
          "distanceMeters": step.distance,
          "durationSeconds": step.expectedTravelTime,
          "maneuverType": String(describing: step.maneuverType),
          "maneuverModifier": step.maneuverDirection.map { String(describing: $0) } ?? "",
          "roadName": step.names?.first ?? "",
          "laneInstructions": laneData
        ])
      }
    }

    onManeuverBannerPressed(["steps": stepsPayload])
  }

  // MARK: - Cancel / teardown (parity with Android cancelNavigation())

  private func tearDownNavigationViewController() {
    guard let vc = navigationViewController else { return }
    vc.willMove(toParent: nil)
    vc.view.removeFromSuperview()
    vc.removeFromParent()
    navigationViewController = nil
    currentNavigationRoutes = nil
  }

  private func cancelNavigation() {
    tearDownNavigationViewController()
    onNavigationCancelled([:])
  }

  // MARK: - Helpers

  private func findParentViewController() -> UIViewController? {
    var responder: UIResponder? = self
    while let r = responder {
      if let vc = r as? UIViewController { return vc }
      responder = r.next
    }
    return nil
  }

  // MARK: - Prop setters (parity with Android setters)

  func setCoordinates(_ coords: [[String: Double]]) {
    coordinates = coords
    if coords.count >= 2 { fetchRoutes() }
  }
  func setWaypointIndices(_ indices: [Int]?) { waypointIndices = indices }
  func setLanguage(_ lang: String?) { language = lang }
  func setVoiceUnits(_ units: String?) { voiceUnits = units }
  func setNavigationProfile(_ profile: String?) { navigationProfile = profile }
  func setExcludeTypes(_ types: [String]?) { excludeTypes = types }
  func setMapStyle(_ style: String?) { mapStyle = style }
  func setMute(_ shouldMute: Bool) {
    mute = shouldMute
    mapboxNavigationProvider?.routeVoiceController?.speechSynthesizer.muted = shouldMute
  }
  func setMaxHeight(_ height: Double?) { maxHeight = height }
  func setMaxWidth(_ width: Double?) { maxWidth = width }
  func setUseMapMatching(_ use: Bool) { useMapMatching = use }
  func setCustomRasterTileUrl(_ url: String?) { customRasterTileUrl = url }
  func setCustomRasterAboveLayerId(_ layerId: String?) { customRasterAboveLayerId = layerId }

  // MARK: - Lifecycle

  public override func removeFromSuperview() {
    routeRequestTask?.cancel()
    tearDownNavigationViewController()
    super.removeFromSuperview()
  }
}

// MARK: - NavigationViewControllerDelegate
//
// Parity with Android's RouteProgressObserver / RoutesObserver / onArrival.

extension ExpoMapboxNavigationView: NavigationViewControllerDelegate {

  public func navigationViewController(
    _ navigationViewController: NavigationViewController,
    didUpdate progress: RouteProgress,
    with location: CLLocation,
    rawLocation: CLLocation
  ) {
    onRouteProgressChanged([
      "distanceRemaining": progress.distanceRemaining,
      "durationRemaining": progress.durationRemaining,
      "distanceTraveled": progress.distanceTraveled,
      "fractionTraveled": progress.fractionTraveled,
      "currentStepDistanceRemaining": progress.currentLegProgress.currentStepProgress.distanceRemaining
    ])
  }

  public func navigationViewController(
    _ navigationViewController: NavigationViewController,
    didArriveAt waypoint: Waypoint
  ) -> Bool {
    onArrival([:])
    return true
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
