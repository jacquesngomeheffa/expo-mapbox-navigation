// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXExpected<__covariant Value, __covariant Error>;

@class MBNNMatchedRouteObject;
@class MBNNRouteObjectMatcherError;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The listener should be set to `RouteObjectMatcher` before using `RouteObjectMatcher`.
 */
NS_SWIFT_NAME(RouteObjectMatcherListener)
@protocol MBNNRouteObjectMatcherListener
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Called if a batch of route objects was matched or errors occurred.
 */
- (void)onRouteObjectsMatchedForRouteObjects:(nonnull MBXExpected<NSArray<MBNNMatchedRouteObject *> *, NSArray<MBNNRouteObjectMatcherError *> *> *)routeObjects;
/** Called if matching a batch of route objects with the specified IDs was cancelled. */
- (void)onMatchingCancelledForIds:(nonnull NSArray<NSString *> *)ids;
@end
