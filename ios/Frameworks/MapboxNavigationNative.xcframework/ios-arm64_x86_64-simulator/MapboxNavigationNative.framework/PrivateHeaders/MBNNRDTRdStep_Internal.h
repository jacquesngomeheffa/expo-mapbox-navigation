// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdManeuver;
@class MBNNRDTRdRoadShield;
@class MBNNRDTRdVoiceInstruction;
@protocol MBNNRDTRdCoordinateArray;
@protocol MBNNRDTRdIntersectionArray;
typedef NS_ENUM(NSInteger, MBNNRDTRdDrivingSide);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Route step description.
 */
NS_SWIFT_NAME(RdStep)
@protocol MBNNRDTRdStep
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Maneuver details for this step.
 */
- (nonnull MBNNRDTRdManeuver *)maneuver;
/** The distance traveled in meters from the maneuver to the next route step. */
- (double)distance;
/** The estimated time traveled in seconds from the maneuver to the next route step. */
- (double)duration;
/** The name of the road or path that forms part of the route step. */
- (nonnull NSString *)name;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The full route geometry from this route step to the next route step.
 */
- (nonnull id<MBNNRDTRdCoordinateArray>)geometry;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The legal driving side at the location for this step. Either "left" or "right".
 */
- (MBNNRDTRdDrivingSide)drivingSide;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * An array of objects representing all the intersections along the step.
 */
- (nonnull id<MBNNRDTRdIntersectionArray>)intersections;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Voice instructions that should be announced along the step.
 */
- (nullable NSArray<MBNNRDTRdVoiceInstruction *> *)voiceInstructions;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Describes a road shield information for the after maneuver road
 */
- (nullable MBNNRDTRdRoadShield *)roadShield;
@end
