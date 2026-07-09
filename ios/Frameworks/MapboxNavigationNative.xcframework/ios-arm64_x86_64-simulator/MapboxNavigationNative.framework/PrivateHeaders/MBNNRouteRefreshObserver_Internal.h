// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteIdentifier;
@class MBNNRouteRefreshError;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Listener for routes refresh events
 */
NS_SWIFT_NAME(RouteRefreshObserver)
@protocol MBNNRouteRefreshObserver
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Will be called if route refresh succeeded
 * @param  routeIdentifier  Identifier of the route being refreshed, containing UUID and index of the route in the original routes response
 * @param  routeRefreshResponse  A string containing the json response from Directions Refresh API that represents refreshed route leg
 * @param  legIndex  Index of the refreshed leg in the route with index routeIndex
 * @param routeGeometryIndex Index in the route geometry, indicates the first geometry to which the annotations are applied
 */
- (void)onRouteRefreshAnnotationsUpdatedForRouteIdentifier:(nonnull MBNNRouteIdentifier *)routeIdentifier
                                      routeRefreshResponse:(nonnull NSString *)routeRefreshResponse
                                                  legIndex:(uint32_t)legIndex
                                        routeGeometryIndex:(uint32_t)routeGeometryIndex;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Will be called in case route refresh was cancelled
 * @param  routeIdentifier  Identifier of the route being refreshed, containing UUID and route index
 */
- (void)onRouteRefreshCancelledForRouteIdentifier:(nonnull MBNNRouteIdentifier *)routeIdentifier;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Will be called in case route refresh was failed
 * @param  routeIdentifier  Identifier of the route being refreshed, containing UUID and route index
 * @param  error  details of the failure
 */
- (void)onRouteRefreshFailedForRouteIdentifier:(nonnull MBNNRouteIdentifier *)routeIdentifier
                                         error:(nonnull MBNNRouteRefreshError *)error;
@end
