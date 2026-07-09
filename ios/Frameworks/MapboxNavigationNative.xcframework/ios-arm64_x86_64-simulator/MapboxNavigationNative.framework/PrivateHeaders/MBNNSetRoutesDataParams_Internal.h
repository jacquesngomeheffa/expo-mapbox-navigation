// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNSetRoutesDataParams.h>

@protocol MBNNRoutesData;

@interface MBNNSetRoutesDataParams ()
- (nonnull instancetype)initWithRoutes:(nonnull id<MBNNRoutesData>)routes
                              legIndex:(uint32_t)legIndex;
@property (nonatomic, readonly, nonnull) id<MBNNRoutesData> routes;
@end
