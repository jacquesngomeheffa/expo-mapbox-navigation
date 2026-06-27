import ExpoModulesCore
import MapboxNavigationCore
import MapboxNavigationUIKit
import MapboxMaps
import MapboxDirections

class ExpoMapboxNavigationView: ExpoView {

  // ─── Props ──────────────────────────────────────────────────────────────
  var coordinates: [[String: Double]] = []
  var waypointIndices: [Int] = []
  var useRouteMatchingApi: Bool = false
  var locale: String = ""
  var routeProfile: String = "mapbox/driving-traffic"
  var routeExcludeList: [String] = []
  var mapStyle: String? = nil
  var mute: Bool = false
  var vehicleMaxHeight: Double? = nil
  var vehicleMaxWidth: Double? = nil
  var customRasterSourceUrl: String? = nil
  var placeCustomRasterLayerAbove: String? = nil
  var disableAlternativeRoutes: Bool = false
  var showsEndOfRouteFeedback: Bool = false

  // ─── Events ─────────────────────────────────────────────────────────────
  let onRouteProgressChanged = EventDispatcher()
  let onWaypointArrival = EventDispatcher()
  let onFinalDestinationArrival = EventDispatcher()
  let onCancelNavigation = EventDispatcher()
  let onRouteChanged = EventDispatcher()
  let onUserOffRoute = EventDispatcher()
  let onRoutesLoaded = EventDispatcher()

  // ─── Internal ───────────────────────────────────────────────────────────
  private var navigationViewController: NavigationViewController?
  private var isNavigationStarted = false
  private var currentWaypointIndex = 0
  private var totalWaypoints = 0

  required init(appContext: AppContext? = nil) {
    super.init(appContext: appContext)
  }

  func startNavigationIfReady() {
    guard coordinates.count >= 2, !isNavigationStarted else { return }
    isNavigationStarted = true
    setupNavigation()
  }

  override func layoutSubviews() {
    super.layoutSubviews()
    startNavigationIfReady()
  }

  // ─── Navigation Setup ───────────────────────────────────────────────────

  private func setupNavigation() {
    // Construire les waypoints
    var allWaypoints: [Waypoint] = []

    for (index, coord) in coordinates.enumerated() {
      guard let lat = coord["latitude"], let lng = coord["longitude"] else { continue }
      let location = CLLocationCoordinate2D(latitude: lat, longitude: lng)
      let wp = Waypoint(coordinate: location)

      // Marquer les waypoints intermédiaires vs simples pass-through
      if !waypointIndices.isEmpty {
        wp.separatesLegs = waypointIndices.contains(index)
      } else {
        wp.separatesLegs = true
      }

      allWaypoints.append(wp)
    }

    totalWaypoints = allWaypoints.filter { $0.separatesLegs }.count

    // Choisir le profil de routing
    let profile: ProfileIdentifier
    switch routeProfile {
    case "mapbox/driving": profile = .automobile
    case "mapbox/walking": profile = .walking
    case "mapbox/cycling": profile = .cycling
    default: profile = .automobileAvoidingTraffic
    }

    // Options de route
    let routeOptions = NavigationRouteOptions(waypoints: allWaypoints, profileIdentifier: profile)

    let resolvedLocale = locale.isEmpty ? Locale.current.identifier : locale
    routeOptions.locale = Locale(identifier: resolvedLocale)
    routeOptions.includesSpokenInstructions = true
    routeOptions.includesVisualInstructions = true

    // Exclusions
    if !routeExcludeList.isEmpty {
      routeOptions.roadClassesToAvoid = buildRoadClassesToAvoid(routeExcludeList)
    }

    // Restrictions de véhicule
    if let maxH = vehicleMaxHeight {
      routeOptions.maximumHeight = Measurement(value: maxH, unit: .meters)
    }
    if let maxW = vehicleMaxWidth {
      routeOptions.maximumWidth = Measurement(value: maxW, unit: .meters)
    }

    if disableAlternativeRoutes {
      routeOptions.includesAlternativeRoutes = false
    }

    // Calcul de la route
    Directions.shared.calculate(routeOptions) { [weak self] (_, result) in
      guard let self = self else { return }
      switch result {
      case .failure(let error):
        print("[ExpoMapboxNavigation] Erreur calcul route: \(error.localizedDescription)")
      case .success(let response):
        guard let routes = response.routes, !routes.isEmpty else { return }
        self.onRoutesLoaded([:])
        self.presentNavigationUI(routes: routes, options: routeOptions)
      }
    }
  }

  private func presentNavigationUI(routes: [Route], options: NavigationRouteOptions) {
    DispatchQueue.main.async { [weak self] in
      guard let self = self else { return }

      let navigationService = MapboxNavigationService(
        routes: routes,
        customRoutingProvider: NavigationSettings.shared.directions,
        credentials: NavigationSettings.shared.directions.credentials,
        simulating: .onPoorGPS
      )

      let navigationOptions = NavigationOptions(navigationService: navigationService)
      let navVC = NavigationViewController(
        for: routes,
        routeIndex: 0,
        routeOptions: options,
        navigationOptions: navigationOptions
      )

      navVC.delegate = self
      navVC.showsEndOfRouteFeedback = self.showsEndOfRouteFeedback
      navVC.showsReportFeedback = false

      if self.mute {
        navVC.navigationService.router.reroutesProactively = false
      }

      self.navigationViewController = navVC

      guard let parentVC = self.parentViewController else { return }
      parentVC.addChild(navVC)
      navVC.view.frame = self.bounds
      navVC.view.autoresizingMask = [.flexibleWidth, .flexibleHeight]
      self.addSubview(navVC.view)
      navVC.didMove(toParent: parentVC)
    }
  }

  private func buildRoadClassesToAvoid(_ list: [String]) -> RoadClasses {
    var classes = RoadClasses()
    for item in list {
      switch item {
      case "toll": classes.insert(.toll)
      case "ferry": classes.insert(.ferry)
      case "motorway": classes.insert(.motorway)
      default: break
      }
    }
    return classes
  }

  private var parentViewController: UIViewController? {
    var responder: UIResponder? = self
    while let r = responder {
      if let vc = r as? UIViewController { return vc }
      responder = r.next
    }
    return nil
  }
}

// ─── NavigationViewControllerDelegate ───────────────────────────────────────

extension ExpoMapboxNavigationView: NavigationViewControllerDelegate {

  func navigationViewController(
    _ vc: NavigationViewController,
    didUpdate progress: RouteProgress,
    with location: CLLocation,
    rawLocation: CLLocation
  ) {
    onRouteProgressChanged([
      "distanceRemaining": progress.currentLegProgress.distanceRemaining,
      "distanceTraveled": progress.distanceTraveled,
      "durationRemaining": progress.durationRemaining,
      "fractionTraveled": progress.fractionTraveled
    ])

    // Détection hors route
    if progress.currentLegProgress.userHasArrivedAtWaypoint {
      currentWaypointIndex += 1
      if currentWaypointIndex < totalWaypoints - 1 {
        onWaypointArrival([
          "distanceRemaining": progress.currentLegProgress.distanceRemaining,
          "distanceTraveled": progress.distanceTraveled,
          "durationRemaining": progress.durationRemaining,
          "fractionTraveled": progress.fractionTraveled
        ])
      }
    }
  }

  func navigationViewControllerDidFinishRouting(_ vc: NavigationViewController) {
    onFinalDestinationArrival([:])
  }

  func navigationViewControllerDidDismiss(_ vc: NavigationViewController, byCanceling canceled: Bool) {
    if canceled {
      onCancelNavigation([:])
    }
  }

  func navigationViewController(_ vc: NavigationViewController, didRerouteAlong route: Route) {
    onRouteChanged([:])
  }

  func navigationViewController(_ vc: NavigationViewController, shouldRerouteFrom location: CLLocation) -> Bool {
    onUserOffRoute([:])
    return true
  }
}
