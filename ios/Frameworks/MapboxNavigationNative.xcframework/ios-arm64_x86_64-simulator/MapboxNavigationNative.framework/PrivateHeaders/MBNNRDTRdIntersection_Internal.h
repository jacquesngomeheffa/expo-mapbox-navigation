// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@class MBNNRDTRdLane;
typedef NS_ENUM(NSInteger, MBNNRDTRdRoadClass);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Object representing an intersection along the step
 */
NS_SWIFT_NAME(RdIntersection)
@protocol MBNNRDTRdIntersection
/**
 * The zero-based index into the geometry, relative to the start of the leg it's on. This value can be used to apply
 * the duration annotation that corresponds with the intersection. Only available on the driving, driving-traffic, and walking profile.
 */
- (nullable NSNumber *)geometryIndex;
/** Coordinate describing the location of the turn */
- (CLLocationCoordinate2D)location;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * An array of Lane objects that represent the available turn lanes at the intersection.
 */
- (nullable NSArray<MBNNRDTRdLane *> *)lanes;
/**
 * A list of bearing values that are available at the intersection.
 * The bearings describe all available roads at the intersection.
 */
- (nullable NSArray<NSNumber *> *)bearings;
/**
 * A list of entry flags, corresponding with the entries in bearings. If the flag is true,
 * indicates that the respective road could be entered on a valid route. If false, the turn onto
 * the respective road would violate a restriction.
 */
- (nullable NSArray<NSNumber *> *)entry;
/** Indicates whether there is a railway crossing at the intersection. */
- (BOOL)railwayCrossing;
/** The index in the bearings and entry arrays. Used to calculate the bearing before the turn. */
- (nullable NSNumber *)inIndex;
/** The index in the bearings and entry arrays. Used to extract the bearing after the turn. */
- (nullable NSNumber *)outIndex;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Classes of the roads exiting the intersection.
 */
- (nullable NSArray<NSNumber *> *)classes;
@end
