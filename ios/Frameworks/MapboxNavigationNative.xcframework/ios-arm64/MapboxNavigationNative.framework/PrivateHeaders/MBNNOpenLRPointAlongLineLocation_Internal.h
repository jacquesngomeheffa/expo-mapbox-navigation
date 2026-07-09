// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@class MBNNGraphPosition;
typedef NS_ENUM(NSInteger, MBNNOrientation);
typedef NS_ENUM(NSInteger, MBNNSideOfRoad);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *  Defines a point along a line on the road graph
 */
NS_SWIFT_NAME(OpenLRPointAlongLineLocation)
__attribute__((visibility ("default")))
@interface MBNNOpenLRPointAlongLineLocation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Position of the point on the graph
 */
- (nonnull MBNNGraphPosition *)getPosition __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Specifies on which side of road locates the point
 */
- (MBNNSideOfRoad)getSideOfRoad;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Specifies orientation
 */
- (MBNNOrientation)getOrientation;
/** Map coordinate of the point */
- (CLLocationCoordinate2D)getCoordinate;

@end
