// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdRouteAnnotation;
@class MBNNRDTRdRouteData;
@protocol MBNNRouteInterface;

NS_SWIFT_NAME(RouteDataAccessor)
__attribute__((visibility ("default")))
@interface MBNNRouteDataAccessor : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

+ (nonnull MBNNRDTRdRouteData *)getRouteDataForRoute:(nonnull id<MBNNRouteInterface>)route __attribute((ns_returns_retained));
+ (nonnull MBNNRDTRdRouteAnnotation *)getRouteAnnotationForRoute:(nonnull id<MBNNRouteInterface>)route __attribute((ns_returns_retained));

@end
