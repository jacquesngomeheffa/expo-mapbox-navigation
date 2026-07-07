// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNSetRoutesResult.h>

@interface MBNNSetRoutesResult ()
- (nonnull instancetype)initWithPrimaryRoute:(nullable id<MBNNRouteInterface>)primaryRoute
                                alternatives:(nonnull NSArray<MBNNRouteAlternative *> *)alternatives;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteAlternative *> *alternatives;
@end
