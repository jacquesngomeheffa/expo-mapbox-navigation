// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Represents lane indications derived from road signs, markings, or both.
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNLaneIndication)
{
    MBNNLaneIndicationNone,
    MBNNLaneIndicationStraight,
    MBNNLaneIndicationSharpLeft,
    MBNNLaneIndicationLeft,
    MBNNLaneIndicationSlightLeft,
    MBNNLaneIndicationSlightRight,
    MBNNLaneIndicationRight,
    MBNNLaneIndicationSharpRight,
    MBNNLaneIndicationUTurn
} NS_SWIFT_NAME(LaneIndication);

NSString* MBNNLaneIndicationToString(MBNNLaneIndication lane_indication);
