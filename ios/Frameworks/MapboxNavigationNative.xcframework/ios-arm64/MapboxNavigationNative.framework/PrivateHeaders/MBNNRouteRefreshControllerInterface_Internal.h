// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@protocol MBNNRouteRefreshObserver;

NS_SWIFT_NAME(RouteRefreshControllerInterface)
@protocol MBNNRouteRefreshControllerInterface
- (void)addObserverForObserver:(nonnull id<MBNNRouteRefreshObserver>)observer;
- (void)removeObserverForObserver:(nonnull id<MBNNRouteRefreshObserver>)observer;
/** Remove all observers that was added by addObserver */
- (void)removeAllObservers;
@end
