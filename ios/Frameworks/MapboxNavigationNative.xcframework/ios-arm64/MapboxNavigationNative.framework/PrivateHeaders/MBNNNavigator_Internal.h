// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNNavigator.h>
#import <MapboxNavigationNative/MBNNRefreshRouteCallback_Internal.h>
#import <MapboxNavigationNative/MBNNRouterType_Internal.h>
#import <MapboxNavigationNative/MBNNSetAlternativeRoutesCallback_Internal.h>
#import <MapboxNavigationNative/MBNNSetRouteCallback_Internal.h>

@interface MBNNNavigator ()
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder;

- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction;
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction
                         inputsService:(nullable MBNNInputsServiceHandle *)inputsService;
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction
                         inputsService:(nullable MBNNInputsServiceHandle *)inputsService
                    adasisFacadeHandle:(nullable MBNNAdasisFacadeHandle *)adasisFacadeHandle;
- (nonnull instancetype)initWithConfig:(nonnull MBNNConfigHandle *)config
                                 cache:(nonnull MBNNCacheHandle *)cache
                       historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                 routerTypeRestriction:(MBNNRouterType)routerTypeRestriction
                         inputsService:(nullable MBNNInputsServiceHandle *)inputsService
                    adasisFacadeHandle:(nullable MBNNAdasisFacadeHandle *)adasisFacadeHandle
                          offlineCache:(nullable MBNNCacheHandle *)offlineCache;
- (nonnull MBNNConfigHandle *)config __attribute((ns_returns_retained));
- (void)addRerouteObserverForObserver:(nonnull id<MBNNRerouteObserver>)observer;
- (void)removeRerouteObserverForObserver:(nonnull id<MBNNRerouteObserver>)observer;
- (void)setRoutesForParams:(nullable MBNNSetRoutesParams *)params
                    reason:(MBNNSetRoutesReason)reason
                  callback:(nonnull MBNNSetRouteCallback)callback;
- (void)setRoutesDataForParams:(nullable MBNNSetRoutesDataParams *)params
                        reason:(MBNNSetRoutesReason)reason
                      callback:(nonnull MBNNSetRouteCallback)callback;
- (void)setAlternativeRoutesForRoutes:(nonnull NSArray<id<MBNNRouteInterface>> *)routes
                             callback:(nonnull MBNNSetAlternativeRoutesCallback)callback;
- (void)refreshRouteForRouteRefreshStr:(nonnull NSString *)routeRefreshStr
                               routeId:(nonnull NSString *)routeId
                         geometryIndex:(uint32_t)geometryIndex
                              callback:(nonnull MBNNRefreshRouteCallback)callback __attribute__((deprecated));
- (void)refreshRouteForRouteRefreshDataRef:(nullable MBXDataRef *)routeRefreshDataRef
                                   routeId:(nonnull NSString *)routeId
                             geometryIndex:(uint32_t)geometryIndex
                                  callback:(nonnull MBNNRefreshRouteCallback)callback __attribute__((deprecated));
- (void)updateLocationForFixLocation:(nonnull MBNNFixLocation *)fixLocation
                            callback:(nonnull MBNNUpdateLocationCallback)callback;
- (void)setElectronicHorizonObserverForObserver:(nullable id<MBNNElectronicHorizonObserver>)observer;
- (void)setElectronicHorizonOptionsForOptions:(nullable MBNNElectronicHorizonOptions *)options;
- (nonnull MBNNRoadObjectsStore *)roadObjectStore __attribute((ns_returns_retained));
- (nonnull MBNNPredictiveCacheController *)createPredictiveCacheControllerForTileStore:(nonnull MBXTileStore *)tileStore
                                                                           descriptors:(nonnull NSArray<MBXTilesetDescriptor *> *)descriptors
                                                                locationTrackerOptions:(nonnull MBNNPredictiveLocationTrackerOptions *)locationTrackerOptions __attribute((ns_returns_retained));
- (nonnull MBNNPredictiveCacheController *)createPredictiveCacheControllerForTileStore:(nonnull MBXTileStore *)tileStore
                                                                          cacheOptions:(nonnull MBNNPredictiveCacheControllerOptions *)cacheOptions
                                                                locationTrackerOptions:(nonnull MBNNPredictiveLocationTrackerOptions *)locationTrackerOptions __attribute((ns_returns_retained));
- (nonnull MBNNPredictiveCacheController *)createPredictiveCacheControllerForCacheOptions:(nonnull MBNNPredictiveCacheControllerOptions *)cacheOptions
                                                                   locationTrackerOptions:(nonnull MBNNPredictiveLocationTrackerOptions *)locationTrackerOptions __attribute((ns_returns_retained));
- (nonnull MBNNPredictiveCacheController *)createPredictiveCacheControllerForLocationTrackerOptions:(nonnull MBNNPredictiveLocationTrackerOptions *)locationTrackerOptions __attribute((ns_returns_retained)) __attribute__((deprecated));
- (void)setRerouteControllerForController:(nonnull id<MBNNRerouteControllerInterface>)controller __attribute__((deprecated));
- (nullable id<MBNNRerouteControllerInterface>)getRerouteController __attribute((ns_returns_retained));
- (nullable id<MBNNRerouteDetectorInterface>)getRerouteDetector __attribute((ns_returns_retained));
- (void)addRouteRefreshObserverForObserver:(nonnull id<MBNNRouteRefreshObserver>)observer;
- (void)removeRouteRefreshObserverForObserver:(nonnull id<MBNNRouteRefreshObserver>)observer;
- (nonnull id<MBNNRouteAlternativesControllerInterface>)getRouteAlternativesController __attribute((ns_returns_retained));
- (nonnull id<MBNNExperimental>)getExperimental __attribute((ns_returns_retained));
- (nonnull id<MBNNTelemetry>)getTelemetryForEventsMetadataProvider:(nonnull id<MBNNEventsMetadataInterface>)eventsMetadataProvider __attribute((ns_returns_retained));
- (nullable id<MBNNLaneGraphAccessor>)getLaneGraphAccessor __attribute((ns_returns_retained));
- (nonnull id<MBNNRouterInterface>)getRouter __attribute((ns_returns_retained));
- (nonnull NSArray<MBNNRouteAlternative *> *)getAlternativeRoutes __attribute((ns_returns_retained));
@end
