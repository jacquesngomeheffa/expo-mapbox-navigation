// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNRoutesChangeInfo.h>

@class MBNNRouteAlternative;
@protocol MBNNRouteInterface;
typedef NS_ENUM(NSInteger, MBNNAlternativeRoutesChangeReason);
typedef NS_ENUM(NSInteger, MBNNPrimaryRouteChangeReason);

@interface MBNNRoutesChangeInfo ()
- (nonnull instancetype)initWithPrimaryRouteChangeReason:(nullable NSNumber *)primaryRouteChangeReason
                                            primaryRoute:(nullable id<MBNNRouteInterface>)primaryRoute
                           alternativeRoutesChangeReason:(nullable NSNumber *)alternativeRoutesChangeReason
                                       alternativeRoutes:(nonnull NSArray<MBNNRouteAlternative *> *)alternativeRoutes;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * New alternative routes.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteAlternative *> *alternativeRoutes;

@end
