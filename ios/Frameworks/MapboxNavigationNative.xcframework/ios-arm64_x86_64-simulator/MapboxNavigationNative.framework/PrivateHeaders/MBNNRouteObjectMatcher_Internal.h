// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNMatchableRouteObject;
@protocol MBNNRouteObjectMatcherListener;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The interface that performs route objects matching to the HD graph.
 */
NS_SWIFT_NAME(RouteObjectMatcher)
@protocol MBNNRouteObjectMatcher
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Sets the listener for obtaining matching results.
 * Must be called before any further `matchRouteObjects` calls.
 */
- (void)setListenerForListener:(nullable id<MBNNRouteObjectMatcherListener>)listener;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Schedules a batch of route objects to match.
 */
- (void)matchRouteObjectsForRouteObjects:(nonnull NSArray<MBNNMatchableRouteObject *> *)routeObjects;
/** Cancels a batch of previously scheduled objects matching. */
- (void)cancelForIds:(nonnull NSArray<NSString *> *)ids;
/** Cancels all scheduled matchings. */
- (void)cancelAll;
@end
