// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNRefreshRouteResult.h>

@class MBNNRouteAlternative;
@protocol MBNNRouteInterface;

@interface MBNNRefreshRouteResult ()
- (nonnull instancetype)initWithRoute:(nonnull id<MBNNRouteInterface>)route
                         alternatives:(nonnull NSArray<MBNNRouteAlternative *> *)alternatives;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Alternative routes with recalculated `duration` info according to the new annotations.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteAlternative *> *alternatives;

@end
