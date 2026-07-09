// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXDataRef;

@class MBNNRerouteError;
@protocol MBNNRouteInterface;
typedef NS_ENUM(NSInteger, MBNNRouterOrigin);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * An observer interface to notify on re-route events
 */
NS_SWIFT_NAME(RerouteObserver)
@protocol MBNNRerouteObserver
/**
 * Called when a re-route is detected.
 * @param routeRequest The route request string containing the original route options.
 * If returned `false` `RerouteControllerInterface.reroute` will not be invoked.
 */
- (BOOL)onRerouteDetectedForRouteRequest:(nonnull NSString *)routeRequest;
/**
 * Called when a re-route is received.
 * @param routeResponse The route response string containing the new route.
 * @param routeRequest The route request string containing the original route options.
 * @param origin The origin of the reroute.
 */
- (void)onRerouteReceivedForRouteResponse:(nonnull MBXDataRef *)routeResponse
                             routeRequest:(nonnull NSString *)routeRequest
                                   origin:(MBNNRouterOrigin)origin;
/** Called when a re-route is cancelled. */
- (void)onRerouteCancelled;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Called when a re-route fails.
 * @param error The error object containing the error message and type.
 */
- (void)onRerouteFailedForError:(nonnull MBNNRerouteError *)error;
/**
 * Called when a re-route is finished.
 * @param route The new route.
 */
- (void)onSwitchToAlternativeForRoute:(nonnull id<MBNNRouteInterface>)route
                             legIndex:(uint32_t)legIndex;
@end
