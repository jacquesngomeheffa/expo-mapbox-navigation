// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <MapboxNavigationNative/MBNNOrientation_Internal.h>
#import <MapboxNavigationNative/MBNNSideOfRoad_Internal.h>

@class MBNNGraphPosition;

NS_SWIFT_NAME(OpenLRPointAlongLineLocation)
__attribute__((visibility ("default")))
@interface MBNNOpenLRPointAlongLineLocation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull MBNNGraphPosition *)getPosition __attribute((ns_returns_retained));
- (MBNNSideOfRoad)getSideOfRoad;
- (MBNNOrientation)getOrientation;
/** Map coordinate of the point */
- (CLLocationCoordinate2D)getCoordinate;

@end
