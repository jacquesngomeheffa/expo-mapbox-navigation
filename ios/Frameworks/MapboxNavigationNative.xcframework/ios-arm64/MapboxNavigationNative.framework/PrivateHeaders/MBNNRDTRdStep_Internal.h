// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRDTRdDrivingSide_Internal.h>

@class MBNNRDTRdCoordinateArray;
@class MBNNRDTRdIntersection;
@class MBNNRDTRdManeuver;
@class MBNNRDTRdRoadShield;
@class MBNNRDTRdVoiceInstruction;

NS_SWIFT_NAME(RdStep)
__attribute__((visibility ("default")))
@interface MBNNRDTRdStep : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull MBNNRDTRdManeuver *)maneuver __attribute((ns_returns_retained));
/** The distance traveled in meters from the maneuver to the next route step. */
- (double)distance;
/** The estimated time traveled in seconds from the maneuver to the next route step. */
- (double)duration;
/** The name of the road or path that forms part of the route step. */
- (nonnull NSString *)name __attribute((ns_returns_retained));
- (nonnull MBNNRDTRdCoordinateArray *)geometry __attribute((ns_returns_retained));
- (MBNNRDTRdDrivingSide)drivingSide;
- (nonnull NSArray<MBNNRDTRdIntersection *> *)intersections __attribute((ns_returns_retained));
- (nullable NSArray<MBNNRDTRdVoiceInstruction *> *)voiceInstructions __attribute((ns_returns_retained));
- (nullable MBNNRDTRdRoadShield *)roadShield __attribute((ns_returns_retained));

@end
