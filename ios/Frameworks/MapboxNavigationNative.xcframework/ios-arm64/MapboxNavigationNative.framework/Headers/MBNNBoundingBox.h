// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

/** Axis-aligned bounding box defined by minimum and maximum coordinates. */
NS_SWIFT_NAME(BoundingBox)
__attribute__((visibility ("default")))
@interface MBNNBoundingBox : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithMin:(CLLocationCoordinate2D)min
                                max:(CLLocationCoordinate2D)max;

/** Minimum (lon, lat) coordinate of the box (lower left corner). */
@property (nonatomic, readonly) CLLocationCoordinate2D min;

/** Maximum (lon, lat) coordinate of the box (upper right corner). */
@property (nonatomic, readonly) CLLocationCoordinate2D max;


@end
