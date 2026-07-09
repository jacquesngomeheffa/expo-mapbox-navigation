// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@protocol MBNNRouteRefreshObserver;

NS_SWIFT_NAME(RouteRefreshControllerInterface)
@protocol MBNNRouteRefreshControllerInterface
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Adds an observer of refresh events (update, fail, cancel)
 * @param  observer  object with corresponding callbacks
 */
- (void)addObserverForObserver:(nonnull id<MBNNRouteRefreshObserver>)observer;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Remove specified observer
 * @param  observer  object that was added by addObserver
 */
- (void)removeObserverForObserver:(nonnull id<MBNNRouteRefreshObserver>)observer;
/** Remove all observers that was added by addObserver */
- (void)removeAllObservers;
@end
