// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Maneuver type.
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNRDTRdManeuverType)
{
    /** A basic turn in the direction of the modifier. */
    MBNNRDTRdManeuverTypeTurn,
    /** The road name changes (after a mandatory turn). */
    MBNNRDTRdManeuverTypeNewName,
    /** Indicates departure from a route leg. */
    MBNNRDTRdManeuverTypeDepart,
    /** Indicates arrival to a destination of a leg. */
    MBNNRDTRdManeuverTypeArrive,
    /** Merge onto a street. */
    MBNNRDTRdManeuverTypeMerge,
    /** Take a ramp to enter a highway. */
    MBNNRDTRdManeuverTypeOnRamp,
    /** Take a ramp to exit a highway. */
    MBNNRDTRdManeuverTypeOffRamp,
    /** Take the left or right side of a fork. */
    MBNNRDTRdManeuverTypeFork,
    /** Road ends in a T intersection. */
    MBNNRDTRdManeuverTypeEndOfRoad,
    /** Continue on a street after a turn. */
    MBNNRDTRdManeuverTypeContinue,
    /** Traverse roundabout. The  modifier specifies the direction of entering the roundabout. */
    MBNNRDTRdManeuverTypeRoundabout,
    /** A traffic circle. */
    MBNNRDTRdManeuverTypeRotary,
    /** A small roundabout that is treated as an intersection. */
    MBNNRDTRdManeuverTypeRoundaboutTurn,
    /** Indicates a change of driving conditions, for example changing the mode from driving to ferry. */
    MBNNRDTRdManeuverTypeNotification,
    /** Indicates the exit maneuver from a roundabout. */
    MBNNRDTRdManeuverTypeExitRoundabout,
    /** Indicates the exit maneuver from a rotary. */
    MBNNRDTRdManeuverTypeExitRotary
} NS_SWIFT_NAME(RdManeuverType);
