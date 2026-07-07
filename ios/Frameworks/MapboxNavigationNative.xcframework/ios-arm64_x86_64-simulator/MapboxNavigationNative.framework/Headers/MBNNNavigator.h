// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNChangeLegCallback.h>
#import <MapboxNavigationNative/MBNNResetCallback.h>
#import <MapboxNavigationNative/MBNNSetRoutesReason.h>
#import <MapboxNavigationNative/MBNNUpdateLocationCallback.h>
@class MBXDataRef;

@class MBNNAdasisFacadeHandle;
@class MBNNCacheHandle;
@class MBNNConfigHandle;
@class MBNNElectronicHorizonOptions;
@class MBNNEvStateData;
@class MBNNFixLocation;
@class MBNNHistoryRecorderHandle;
@class MBNNInputsServiceHandle;
@class MBNNNavigationSessionState;
@class MBNNNavigationStatus;
@class MBNNPredictiveCacheController;
@class MBNNPredictiveCacheControllerOptions;
@class MBNNPredictiveLocationTrackerOptions;
@class MBNNRoadObjectsStore;
@class MBNNRouteAlternative;
@class MBNNSetRoutesDataParams;
@class MBNNSetRoutesParams;
@class MBXTileStore;
@class MBXTilesetDescriptor;
@protocol MBNNElectronicHorizonObserver;
@protocol MBNNEventsMetadataInterface;
@protocol MBNNExperimental;
@protocol MBNNFallbackVersionsObserver;
@protocol MBNNLaneGraphAccessor;
@protocol MBNNNavigatorObserver;
@protocol MBNNRerouteControllerInterface;
@protocol MBNNRerouteDetectorInterface;
@protocol MBNNRerouteObserver;
@protocol MBNNRouteAlternativesControllerInterface;
@protocol MBNNRouteInterface;
@protocol MBNNRouteRefreshObserver;
@protocol MBNNRouterInterface;
@protocol MBNNTelemetry;

NS_SWIFT_NAME(Navigator)
__attribute__((visibility ("default")))
@interface MBNNNavigator : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/** Provides navigator version */
- (nonnull NSString *)version __attribute((ns_returns_retained));
/**
 * Shuts down Navigator instance, so it frees resources.
 * This Navigator instance should not be used after shutdown call.
 */
- (void)shutdown;
/**
 * Sets the `FallbackVersionsObserver` instance for notifications about fallback (available offline) versions found.
 * In order to remove observer just call `setFallbackVersionsObserver(null)`.
 */
- (void)setFallbackVersionsObserverForObserver:(nullable id<MBNNFallbackVersionsObserver>)observer;
/**
 * After setting observer `getStatus` shouldn't be called,
 * but `Navigator` object will start calling `NavigatorObserver.onStatus` as soon as at least one raw location is received.
 */
- (void)addObserverForObserver:(nonnull id<MBNNNavigatorObserver>)observer;
/** Removes the navigator observer */
- (void)removeObserverForObserver:(nonnull id<MBNNNavigatorObserver>)observer;
/**
 * Asynchronously sets leg of the already loaded directions
 * The callback is scheduled using the `common::Scheduler` of the thread calling the `Navigator` constructor.
 *
 * @param leg    new leg index
 * @param  callback Callback which is called when the async operation is completed
 */
- (void)changeLegForLeg:(uint32_t)leg
               callback:(nonnull MBNNChangeLegCallback)callback;
/**
 * Resets internal state of Navigator, useful in case if we use Navigator for replays or simulation,
 * when timestamps can decrease and locations can have unexpected significant changes.
 * Please call it before significant change of location, e.g. when restarting
 * navigation simulation, or before resetting location to not real (simulated)
 * position without recreation of navigator.
 *
 * @param callback If provided `reset()` runs asynchronously and callback is invoked on owning thread.
 *                 If not provided method runs synchronously blocking owning thread.
 */
- (void)resetForCallback:(nullable MBNNResetCallback)callback;
/**
 * Start refreshing routes which were provided by setRoutes call.
 * If you set useInternalRouteRefresh flag in the config refreshing will start automatically
 * after first call of setRoutes.
 * Otherwise you need to start refresh manually by calling this function.
 * Once you have started routes refreshing it will automatically use routes that came from setRoutes,
 * i.e. you don't need to call startRoutesRefresh each time after setRoutes was called.
 * Call is non-blocking.
 *
 * Must be invoked only on the owning thread.
 *
 * @param defaultRefreshPeriodMs  used in case expiration time is ignored or wasn't determined from Directions API response
 * @param ignoreExpirationTime    if you want to refresh routes only each defaultRefreshPeriodInSeconds
 */
- (void)startRoutesRefreshForDefaultRefreshPeriodMs:(uint64_t)defaultRefreshPeriodMs
                               ignoreExpirationTime:(BOOL)ignoreExpirationTime;
/**
 * Stop routes refreshing. If refresh process haven't started do nothing.
 * Call is non-blocking, i.e. refresh observer still can have several updates after stop.
 *
 * Must be invoked only on the owning thread.
 */
- (void)stopRoutesRefresh;
/** Pause navigator. We will no longer receive NavigationStatus while there is a pause. */
- (void)pause;
/** Resume navigator. Remove pause, and start receiving NavigationStatus'es */
- (void)resume;
/**
 * Navigation session encapsulates trip-related data, like id, mode, travelled distance, etc.
 * Start / Stop navigation session methods must be called for correct telemetry events and potentially billing.
 * If navigation session isn't started, no telemetry events will be generated, and navigator functionalities may not work.
 * If there is no current route set, new navigation session is considered as free drive, otherwise active guidance. So,
 * if no free drive session needed on start call `setRoute(...)` before `startNavigationSession()`. If a new route is set
 * (NewRoute reason) or Navigator#resetRoute() is called current drive session ends and a new one starts automatically.
 * Note: navigation session is not automatically started on navigator start and not ended on navigator destruction.
 * Session may outlive navigator destruction, see storeNavigationSession() method description.
 */
- (void)startNavigationSession;
/**
 * Stops NavigationSession. No sessions will be created on setting / resetting routes if session is stopped.
 * For more details see description of `startNavigationSession`
 */
- (void)stopNavigationSession;
/**
 * If the nav-native client needs to restart Navigator without changing trip session, user must call `storeNavigationSession()`
 * before destructing navigator and just after restart set the internal session state back with `restoreNavigationSession(state)`
 * Note: Empty serialized field means null trip session.
 */
- (nonnull MBNNNavigationSessionState *)storeNavigationSession __attribute((ns_returns_retained));
/** See description of `storeNavigationSession` */
- (void)restoreNavigationSessionForState:(nonnull MBNNNavigationSessionState *)state;
/**
 * Invoke when any component of EV data is changed so that it can be used in alternatives requests.
 * You should pass all components of EV data via [data], all the previous values will NOT be cached.
 */
- (void)onEvDataUpdatedForData:(nonnull MBNNEvStateData *)data;
/**
 * Returns the current primary route.
 *
 * Must be invoked only on the owning thread.
 *
 * @return The current primary route. This value is null if there is no current route in the navigator.
 */
- (nullable id<MBNNRouteInterface>)getPrimaryRoute __attribute((ns_returns_retained));
/**
 * Return last navigation status.
 *
 * Must be invoked only on owning thread.
 * Location is invalid (equal to 0.0,0.0) if updateLocation() has not been invoked before.
 */
- (nonnull MBNNNavigationStatus *)getNavigationStatus __attribute((ns_returns_retained));

@end
