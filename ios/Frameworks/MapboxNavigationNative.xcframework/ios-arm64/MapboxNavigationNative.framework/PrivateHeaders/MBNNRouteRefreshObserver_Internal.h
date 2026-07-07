// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteIdentifier;
@class MBNNRouteRefreshError;

NS_SWIFT_NAME(RouteRefreshObserver)
@protocol MBNNRouteRefreshObserver
- (void)onRouteRefreshAnnotationsUpdatedForRouteIdentifier:(nonnull MBNNRouteIdentifier *)routeIdentifier
                                      routeRefreshResponse:(nonnull NSString *)routeRefreshResponse
                                                  legIndex:(uint32_t)legIndex
                                        routeGeometryIndex:(uint32_t)routeGeometryIndex;
- (void)onRouteRefreshCancelledForRouteIdentifier:(nonnull MBNNRouteIdentifier *)routeIdentifier;
- (void)onRouteRefreshFailedForRouteIdentifier:(nonnull MBNNRouteIdentifier *)routeIdentifier
                                         error:(nonnull MBNNRouteRefreshError *)error;
@end
