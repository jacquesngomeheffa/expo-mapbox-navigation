// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteAlternative;
@protocol MBNNRouteInterface;

NS_SWIFT_NAME(RoutesData)
@protocol MBNNRoutesData
- (nonnull id<MBNNRouteInterface>)primaryRoute;
- (nonnull NSArray<MBNNRouteAlternative *> *)alternativeRoutes;
@end
