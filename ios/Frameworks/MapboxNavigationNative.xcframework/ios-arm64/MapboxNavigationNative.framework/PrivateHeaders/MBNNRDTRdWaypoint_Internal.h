// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <MapboxNavigationNative/MBNNRDTRdWaypointType_Internal.h>
@class MBXCoordinate2D;

NS_SWIFT_NAME(RdWaypoint)
__attribute__((visibility ("default")))
@interface MBNNRDTRdWaypoint : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithName:(nonnull NSString *)name
                            location:(CLLocationCoordinate2D)location
                            distance:(nullable NSNumber *)distance
                            metadata:(nullable NSString *)metadata
                              target:(nullable MBXCoordinate2D *)target
                                type:(MBNNRDTRdWaypointType)type;

/**
 *  Field name as is from direction's response.
 *  Or corresponding value from a request URI param `waypoint_names` if there is no waypoints in the route response.
 */
@property (nonatomic, readonly, nonnull, copy) NSString *name;

/** Field location as is from direction's response. */
@property (nonatomic, readonly) CLLocationCoordinate2D location;

/** Field distance as is from direction's response. */
@property (nonatomic, readonly, nullable) NSNumber *distance;

/** Field metadata as is from direction's response. */
@property (nonatomic, readonly, nullable, copy) NSString *metadata;

/** Corresponding value from a route request URI param `waypoint_targets`. */
@property (nonatomic, readonly, nullable) MBXCoordinate2D *target;

@property (nonatomic, readonly) MBNNRDTRdWaypointType type;

- (BOOL)isEqualToRdWaypoint:(nonnull MBNNRDTRdWaypoint *)other;

@end
