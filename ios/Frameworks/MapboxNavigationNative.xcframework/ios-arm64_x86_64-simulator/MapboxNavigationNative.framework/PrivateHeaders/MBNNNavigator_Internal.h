// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNNavigator.h>
#import <MapboxNavigationNative/MBNNGetHDRouteLaneGuidancesCallback_Internal.h>
#import <MapboxNavigationNative/MBNNRefreshRouteCallback_Internal.h>
#import <MapboxNavigationNative/MBNNSetAlternativeRoutesCallback_Internal.h>
#import <MapboxNavigationNative/MBNNSetRouteCallback_Internal.h>

@interface MBNNNavigator ()
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Constructs navigator object with given dependencies
 *
 * @param config                config handle created with `ConfigFactory`
 * @param cache                 cache handle created with `CacheFactory`
 * @param routerTypeRestriction restrict Navigator internal route requests to Online / Onboard in case of passing an appropriate router type.
 * Hybrid means no restriction and used by default.
 * @param historyRecorder       history recorder created with `HistoryRecorderHandle.build` method
 * @param inputsService         inputs service created with 'InputsServiceHandle.build' method
 * @param adasisFacadeHandle    handle to AdasisFacade to access ADASIS functionalities. Optional, if not passed, ADASIS won't work.
 * @param offlineCache          offline cache handle created with `CacheFactory`, will be used by Hybrid router as a fallback in case of routing
 * on current tiles failed
 * @param tilesManager          tiles manager created with 'TilesManagerHandle.build' method
 */
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder __attribute__((deprecated));

- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction __attribute__((deprecated));
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction
                         inputsService:(nullable MBNNInputsServiceHandle *)inputsService __attribute__((deprecated));
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction
                         inputsService:(nullable MBNNInputsServiceHandle *)inputsService
                    adasisFacadeHandle:(nullable MBNNAdasisFacadeHandle *)adasisFacadeHandle __attribute__((deprecated));
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction
                         inputsService:(nullable MBNNInputsServiceHandle *)inputsService
                    adasisFacadeHandle:(nullable MBNNAdasisFacadeHandle *)adasisFacadeHandle
                          offlineCache:(nullable MBNNCacheHandle *)offlineCache __attribute__((deprecated));
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                          tilesManager:(nonnull MBNNTilesManagerHandle *)tilesManager
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder;
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                          tilesManager:(nonnull MBNNTilesManagerHandle *)tilesManager
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction;
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                          tilesManager:(nonnull MBNNTilesManagerHandle *)tilesManager
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction
                         inputsService:(nullable MBNNInputsServiceHandle *)inputsService;
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                          tilesManager:(nonnull MBNNTilesManagerHandle *)tilesManager
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction
                         inputsService:(nullable MBNNInputsServiceHandle *)inputsService
                    adasisFacadeHandle:(nullable MBNNAdasisFacadeHandle *)adasisFacadeHandle;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Obtain config object that was used for Navigator construction
 */
- (nonnull MBNNConfigHandle *)config __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Adds the `RerouteObserver` instance for the observers collection
 * for notifications about reroute events.
 * In order to remove the observer call `removeRerouteObserver(observer)`.
 * Note: Observer must live till the removeRerouteObserver is called.
 */
- (void)addRerouteObserverForObserver:(nonnull id<MBNNRerouteObserver>)observer;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Removes the reroute observer
 */
- (void)removeRerouteObserverForObserver:(nonnull id<MBNNRerouteObserver>)observer;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Sets or resets parsed primary route and alternative routes to the navigator asynchronously.
 * If `params` is not empty -- active guidance will start.
 * To stop active guidance pass a null to the `params` param (free-drive mode entry)
 * The callback is scheduled using the `common::Scheduler` of the thread calling the `Navigator` constructor(callback is called for route reset too).
 * This method should be used to change primary route. To change only alternative routes, use `setAlternativeRoutes` method.
 * Method can provide less alternative routes, then the passed `routes`. See description of `RouteParser#createRoutesData` method.
 * @param  params        Primary and alternative routes to be set.
 * @param  callback      Callback which is called when the async operation is completed.
 * @param  reason        The reason for set/reset the routes
 */
- (void)setRoutesForParams:(nullable MBNNSetRoutesParams *)params
                    reason:(MBNNSetRoutesReason)reason
                  callback:(nonnull MBNNSetRouteCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Sets or resets parsed primary route and alternative routes with calculated metadata to the navigator asynchronously.
 * This method is the same as `Navigator#setRoutes`, but accept alternative routes with already calculated metadata.
 * This method should be used to set `RoutesData` to the `Navigator` to avoid recalculating alternatives metadata.
 * To get `RoutesData` use `RouteParser#createRoutesData`
 *
 * For parameters description see `setRoutes`
 */
- (void)setRoutesDataForParams:(nullable MBNNSetRoutesDataParams *)params
                        reason:(MBNNSetRoutesReason)reason
                      callback:(nonnull MBNNSetRouteCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Sets parsed alternative routes for the navigator asynchronously.
 * The callback is scheduled using the `common::Scheduler` of the thread calling the `Navigator` constructor(callback is called for route reset too).
 * This method should be used to change alternative routes without changing the primary route.
 * Method can provide less alternative routes, then the passed `routes`. See description of `RouteParser#createRoutesData` method.
 * @param  routes         Alternative routes to be set.
 * @param  callback       Callback which is called when the async operation is completed
 */
- (void)setAlternativeRoutesForRoutes:(nonnull NSArray<id<MBNNRouteInterface>> *)routes
                             callback:(nonnull MBNNSetAlternativeRoutesCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Asynchronously updates route with id == `routeId`(i.e. can be alternative route) so that subsequent calls to getStatus will
 * reflect the most current annotations for this route. Also apply updates to the current route and provides result in callback.
 * The callback is scheduled using the `common::Scheduler` of the thread calling the `Navigator` constructor.
 *
 * In the case if number of legs in `routeRefreshStr` is less than number of legs in the route the tail of leg's array is updated,
 * i.e. if route has 10 legs and `routeRefreshStr` has 2 legs, than 1st leg refresh will be applied to 9th leg and  2nd one to the 10th one.
 * We do it this way to properly handle the case when current leg index was passed to Refresh API and this index is > 0. In this case Refresh API doesn't
 * return updates for legs before this index(assuming they are already passed by user)
 *
 * @param  routeRefreshStr  A string containing the json represented route refresh response from `RouterInterface` or Directions Refresh API(or at least should be in the same format and consistent with route being updated)
 * @param  routeId  Which route to apply the annotation update to
 * @param  geometryIndex the geometry index from which the route refresh was requested; pass 0 if requested from the very route start
 * @param  callback Callback which is called when the async operation is completed
 *
 * Note, the method is deprecated and will be removed soon. Please use NN internal route refresh mechanism instead, see startRoutesRefresh / addRouteRefreshObserver
 */
- (void)refreshRouteForRouteRefreshStr:(nonnull NSString *)routeRefreshStr
                               routeId:(nonnull NSString *)routeId
                         geometryIndex:(uint32_t)geometryIndex
                              callback:(nonnull MBNNRefreshRouteCallback)callback __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Asynchronously updates route with id == `routeId`(i.e. can be alternative route) so that subsequent calls to getStatus will
 * reflect the most current annotations for this route. Also apply updates to the current route and provides result in callback.
 * The callback is scheduled using the `common::Scheduler` of the thread calling the `Navigator` constructor.
 *
 * In the case if number of legs in `routeRefreshDataRef` is less than number of legs in the route the tail of leg's array is updated,
 * i.e. if route has 10 legs and `routeRefreshDataRef` has 2 legs, than 1st leg refresh will be applied to 9th leg and  2nd one to the 10th one.
 * We do it this way to properly handle the case when current leg index was passed to Refresh API and this index is > 0. In this case Refresh API doesn't
 * return updates for legs before this index(assuming they are already passed by user)
 *
 * @param  routeRefreshDataRef  A data_ref containing the json represented route refresh response from `RouterInterface` or Directions Refresh API(or at least should be in the same format and consistent with route being updated)
 * @param  routeId  Which route to apply the annotation update to
 * @param  geometryIndex the geometry index from which the route refresh was requested
 * @param  callback Callback which is called when the async operation is completed
 *
 * Note, the method is deprecated and will be removed soon. Please use NN internal route refresh mechanism instead, see startRoutesRefresh / addRouteRefreshObserver
 */
- (void)refreshRouteForRouteRefreshDataRef:(nullable MBXDataRef *)routeRefreshDataRef
                                   routeId:(nonnull NSString *)routeId
                             geometryIndex:(uint32_t)geometryIndex
                                  callback:(nonnull MBNNRefreshRouteCallback)callback __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Asynchronously passes in the current fix location of the user.
 * The callback is scheduled using the `common::Scheduler` of the thread calling the `Navigator` constructor.
 *
 * @param  fix  The current fix location of user.
 * @param  callback Callback which is called when the async operation is completed
 */
- (void)updateLocationForFixLocation:(nonnull MBNNFixLocation *)fixLocation
                            callback:(nonnull MBNNUpdateLocationCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Set an observer and the configuration for the EH.
 */
- (void)setElectronicHorizonObserverForObserver:(nullable id<MBNNElectronicHorizonObserver>)observer;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Sets electronic horizon options. Pass null to reset to defaults.
 */
- (void)setElectronicHorizonOptionsForOptions:(nullable MBNNElectronicHorizonOptions *)options;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns road object store
 */
- (nonnull MBNNRoadObjectsStore *)roadObjectsStore __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Creates predictive cache controller to populate the specified tile store instance
 * with the tiles described by the specified tileset descriptors.
 *
 * @param tileStore               a tile store that should be populated with tiles
 * @param descriptors             a list of tileset descriptors
 * @param locationTrackerOptions  options to configure what tiles should be loaded for current location and route
 *
 * @return a PredictiveCacheController object
 */
- (nonnull MBNNPredictiveCacheController *)createPredictiveCacheControllerForTileStore:(nonnull MBXTileStore *)tileStore
                                                                           descriptors:(nonnull NSArray<MBXTilesetDescriptor *> *)descriptors
                                                                locationTrackerOptions:(nonnull MBNNPredictiveLocationTrackerOptions *)locationTrackerOptions __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @param tileStore               a tile store that should be populated with tiles
 * @param cacheOptions            cache options such as dataset and version of tiles
 * @param locationTrackerOptions  options to configure what tiles should be loaded for current location and route
 *
 * @return a PredictiveCacheController object
 */
- (nonnull MBNNPredictiveCacheController *)createPredictiveCacheControllerForTileStore:(nonnull MBXTileStore *)tileStore
                                                                          cacheOptions:(nonnull MBNNPredictiveCacheControllerOptions *)cacheOptions
                                                                locationTrackerOptions:(nonnull MBNNPredictiveLocationTrackerOptions *)locationTrackerOptions __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Creates predictive cache controller to populate inner tile store instance of this navigator.
 *
 * @param cacheOptions            cache options such as dataset and version of tiles
 * @param locationTrackerOptions  options to configure what tiles should be loaded for current location and route
 *
 * @return a PredictiveCacheController object
 */
- (nonnull MBNNPredictiveCacheController *)createPredictiveCacheControllerForCacheOptions:(nonnull MBNNPredictiveCacheControllerOptions *)cacheOptions
                                                                   locationTrackerOptions:(nonnull MBNNPredictiveLocationTrackerOptions *)locationTrackerOptions __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Creates predictive cache controller to populate inner tile store instance of this navigator
 * and take dataset and version of tiles from Navigator's config.
 *
 * @param locationTrackerOptions  options to configure what tiles should be loaded for current location and route
 *
 * @return a PredictiveCacheController object
 */
- (nonnull MBNNPredictiveCacheController *)createPredictiveCacheControllerForLocationTrackerOptions:(nonnull MBNNPredictiveLocationTrackerOptions *)locationTrackerOptions __attribute((ns_returns_retained)) __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Sets the custom reroute controller to be used by the navigator.
 * If not set, the navigator will use the default reroute controller.
 */
- (void)setRerouteControllerForController:(nonnull id<MBNNRerouteControllerInterface>)controller __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns the currently use reroute controller.
 */
- (nullable id<MBNNRerouteControllerInterface>)getRerouteController __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns the reroute detector, that can be used to force reroute events
 * to the navigator.
 */
- (nullable id<MBNNRerouteDetectorInterface>)getRerouteDetector __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Adds an observer of refresh events (update, fail, cancel).
 * Adding observer doesn't affect refresh mechanism,
 * i.e. it will receive events according to refresh configuration if refresh is running
 * @param  observer  object with corresponding callbacks
 */
- (void)addRouteRefreshObserverForObserver:(nonnull id<MBNNRouteRefreshObserver>)observer;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Remove specified observer
 * @param  observer  object that was added by addRouteRefreshObserver
 */
- (void)removeRouteRefreshObserverForObserver:(nonnull id<MBNNRouteRefreshObserver>)observer;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Get RouteAlternativesController
 */
- (nonnull id<MBNNRouteAlternativesControllerInterface>)getRouteAlternativesController __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns interface implementing experimental APIs
 * Caller must guarantee `Navigator` instance is alive when calling any methods of returned instance
 */
- (nonnull id<MBNNExperimental>)getExperimental __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns interface telemetry APIs
 * Caller must guarantee `Navigator` instance is alive when calling any methods of returned instance,
 * otherwise they will have no effect.
 *
 * @param eventsMetadataProvider Implementation of the EventsMetadataInterface
 *                               that provides relevant metadata for the telemetry.
 * @return TelemetryInterface
 */
- (nonnull id<MBNNTelemetry>)getTelemetryForEventsMetadataProvider:(nonnull id<MBNNEventsMetadataInterface>)eventsMetadataProvider __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns accessor to HD lane graph. Empty only if HD is disabled.
 * This method should be called once at start.
 * Returned `LaneGraphAccessor` could be used anytime, even when HD data is not available
 * (in that case it would return empty data for each request).
 * Note: all HD data is provided in `NavigationStatus.hdMatchingResult`
 */
- (nullable id<MBNNLaneGraphAccessor>)getHDLaneGraphAccessor __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns matcher that performs route objects matching to the HD graph.
 * Empty if HD is disabled.
 */
- (nullable id<MBNNRouteObjectMatcher>)getHDRouteObjectMatcher __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Builds HD route lane guidance within the given bounding box.
 * This operation returns an array of RouteLaneGuidance objects. Each object in this array
 * corresponds to an independent set of route segments within the bounding box (including alternatives).
 * The results are delivered to the callback.
 * Note: This currently works by extending the current HD route match forward.
 * Performance may degrade for bounding boxes that are far from the current match.
 */
- (nonnull id<MBXCancelable>)getHDRouteLaneGuidancesForBBoxForBbox:(nonnull MBNNBoundingBox *)bbox
                                                            bboxId:(nonnull NSString *)bboxId
                                                          callback:(nonnull MBNNGetHDRouteLaneGuidancesCallback)callback __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Return internal navigator's router used for alternative requests, default is hybrid
 */
- (nonnull id<MBNNRouterInterface>)getRouter __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns the current alternative routes.
 *
 * Must be invoked only on the owning thread.
 */
- (nonnull NSArray<MBNNRouteAlternative *> *)getAlternativeRoutes __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns InputsServiceHandle passed to Navigator's constructor.
 */
- (nullable MBNNInputsServiceHandle *)getInputsService __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns current state of charging.
 */
- (MBNNChargingState)stateOfCharging;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Enters Charging state. No alternatives / reroutes / route refresh calls will be made while charging. Call stopCharging to exit this state.
 */
- (void)startCharging;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Exits Charging state.
 * Must be called only after call of startCharging(), and in case the current waypoint is an EV charging station.
 * Next leg will be started automatically, so no `changeRouteLeg()` call please.
 * If changeLegCallback is provided, it will be called when the next leg is started, or immediately if changing leg isn't possible
 */
- (void)stopChargingForChangeLegCallback:(nonnull MBNNChangeLegCallback)changeLegCallback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns a VoiceInstructionsRetriever instance that can be used to retrieve the last voice instruction.
 *
 * This API is experimental and may be changed eventually
 */
- (nonnull id<MBNNVoiceInstructionsRetriever>)getVoiceInstructionsRetriever __attribute((ns_returns_retained));
@end
