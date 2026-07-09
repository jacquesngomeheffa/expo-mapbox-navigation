// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteAlternative;
@protocol MBNNRouteInterface;
typedef NS_ENUM(NSInteger, MBNNAlternativeRoutesChangeReason);
typedef NS_ENUM(NSInteger, MBNNPrimaryRouteChangeReason);

/** Primary and alternative routes change information used in `NavigatorObserver::onRoutesChanged`. */
NS_SWIFT_NAME(RoutesChangeInfo)
__attribute__((visibility ("default")))
@interface MBNNRoutesChangeInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;


/**
 * Contains the reason for the primary route change.
 * If not set, then the primary route has not changed, and `RoutesChangeInfo::primaryRoute` is empty.
 *
 * If the reason is `PrimaryRouteChangeReason::Refresh`, do not assume that the route will be
 * the same as the previous one. The route instance may be new, even if only annotations have changed.
 *
 * If you are caching the route, always update the cache with the latest route provided by this function.
 */
@property (nonatomic, readonly, nullable) NSNumber *primaryRouteChangeReason;

/** New primary route. */
@property (nonatomic, readonly, nullable) id<MBNNRouteInterface> primaryRoute;

/**
 * Contains the reason for the change in alternative routes.
 * If not set, the alternative routes have not changed, and `RoutesChangeInfo::alternativeRoutes` is empty.
 */
@property (nonatomic, readonly, nullable) NSNumber *alternativeRoutesChangeReason;


@end
