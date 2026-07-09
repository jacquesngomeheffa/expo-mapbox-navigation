// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNSetRoutesResult.h>

@class MBNNRouteAlternative;
@protocol MBNNRouteInterface;

@interface MBNNSetRoutesResult ()
- (nonnull instancetype)initWithPrimaryRoute:(nullable id<MBNNRouteInterface>)primaryRoute
                                alternatives:(nonnull NSArray<MBNNRouteAlternative *> *)alternatives;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Additional information about alternative routes relative to the primary route.
 * Routes in the array will be in the same order as in the `SetRoutesParams.alternativeRoutes`.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteAlternative *> *alternatives;

@end
