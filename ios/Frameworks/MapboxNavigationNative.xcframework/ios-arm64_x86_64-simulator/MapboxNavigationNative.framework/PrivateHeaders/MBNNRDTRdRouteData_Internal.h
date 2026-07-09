// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@protocol MBNNRDTRdCoordinateArray;
@protocol MBNNRDTRdLegArray;
@protocol MBNNRDTRdWaypointArray;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *
 * THIS IS AN EXPERIMENTAL API AND SUBJECT TO CHANGE.
 *
 *  RouteData object.
 *  Immutable part of Direction response.
 */
NS_SWIFT_NAME(RdRouteData)
@protocol MBNNRDTRdRouteData
/** The estimated travel time through the waypoints in seconds. */
- (double)duration;
/** The distance traveled through the waypoints in meters. */
- (double)distance;
/**
 * Name of the weight used. The default is routability, which is duration-based, with additional penalties for
 * less desirable maneuvers.
 */
- (nonnull NSString *)weightName;
/** Weight in units described by weightName. */
- (double)weight;
/** The locale used for voice instructions. */
- (nonnull NSString *)voiceLocale;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * An array of route leg objects.
 */
- (nonnull id<MBNNRDTRdLegArray>)legs;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Depending on the overview query parameter, this is the complete route geometry (full), a simplified
 * geometry to the zoom level at which the route can be displayed in full (simplified), or is empty.
 */
- (nonnull id<MBNNRDTRdCoordinateArray>)geometry;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Waypoints from direction's routes response.
 */
- (nonnull id<MBNNRDTRdWaypointArray>)waypoints;
@end
