// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNRoutesChangeInfo.h>

@interface MBNNRoutesChangeInfo ()
- (nonnull instancetype)initWithPrimaryRouteChangeReason:(nullable NSNumber *)primaryRouteChangeReason
                                            primaryRoute:(nullable id<MBNNRouteInterface>)primaryRoute
                           alternativeRoutesChangeReason:(nullable NSNumber *)alternativeRoutesChangeReason
                                       alternativeRoutes:(nonnull NSArray<MBNNRouteAlternative *> *)alternativeRoutes;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteAlternative *> *alternativeRoutes;
@end
