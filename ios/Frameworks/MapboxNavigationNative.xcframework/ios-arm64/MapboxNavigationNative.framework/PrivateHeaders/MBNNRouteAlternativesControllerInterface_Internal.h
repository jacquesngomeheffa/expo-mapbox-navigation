// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteAlternativesOptions;
@protocol MBNNRouteAlternativesObserver;

NS_SWIFT_NAME(RouteAlternativesControllerInterface)
@protocol MBNNRouteAlternativesControllerInterface
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Set RouteAlternativesOptions (can be changed in run-time).
 */
- (void)setRouteAlternativesOptionsForOptions:(nonnull MBNNRouteAlternativesOptions *)options;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Add observer. Requests will not be sent if no one observer present.
 */
- (void)addObserverForObserver:(nonnull id<MBNNRouteAlternativesObserver>)observer;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Remove observer. Requests will not be sent if no one observer present.
 */
- (void)removeObserverForObserver:(nonnull id<MBNNRouteAlternativesObserver>)observer;
/** Remove all observers. Requests will not be sent if no one observer present. */
- (void)removeAllObservers;
@end
