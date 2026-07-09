// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Route waypoint type.
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNRDTRdWaypointType)
{
    /** Waypoint is a beginning or/and ending of a leg, not silent and not EV Charging waypoint. */
    MBNNRDTRdWaypointTypeRegular,
    /**
     * Silent means no breaking into legs in this point and no notification.
     * Silent waypoint doesn't break the route part to legs but must be visited.
     * No alert on reaching silent waypoint.
     */
    MBNNRDTRdWaypointTypeSilent,
    /**
     * Charging stops inserted by EV Routing for routes requiring charging along
     * the way. As these type of waypoints are not requested by user, they may change
     * on reroutes. But they are not silent, i.e. such waypoint is an ending of a leg
     * and a beginning of another. Note, charging requires time, so they effect route duration
     */
    MBNNRDTRdWaypointTypeEvChargingServer,
    /**
     * Charging stop added by User. Unlike `EvChargingServer` this will not be changed on
     * reroutes and cannot be skipped. Mind that charging requires time, calculated on server.
     */
    MBNNRDTRdWaypointTypeEvChargingUser
} NS_SWIFT_NAME(RdWaypointType);

NSString* MBNNRDTRdWaypointTypeToString(MBNNRDTRdWaypointType rd_waypoint_type);
