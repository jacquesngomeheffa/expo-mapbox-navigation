// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Non-default lane type
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNLaneType)
{
    /** https://en.wikipedia.org/wiki/Reversible_lane */
    MBNNLaneTypeReverse,
    /** https://wiki.openstreetmap.org/wiki/Key:lanes:both_ways */
    MBNNLaneTypeBothWays,
    /** https://wiki.openstreetmap.org/wiki/Bus_lanes */
    MBNNLaneTypeBus
} NS_SWIFT_NAME(LaneType);

NSString* MBNNLaneTypeToString(MBNNLaneType lane_type);
