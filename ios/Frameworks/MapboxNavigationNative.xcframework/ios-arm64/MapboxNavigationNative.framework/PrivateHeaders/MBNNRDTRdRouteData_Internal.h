// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdCoordinateArray;
@class MBNNRDTRdLegArray;
@class MBNNRDTRdWaypointArray;

NS_SWIFT_NAME(RdRouteData)
__attribute__((visibility ("default")))
@interface MBNNRDTRdRouteData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/** The estimated travel time through the waypoints in seconds. */
- (double)duration;
/** The distance traveled through the waypoints in meters. */
- (double)distance;
/**
 * Name of the weight used. The default is routability, which is duration-based, with additional penalties for
 * less desirable maneuvers.
 */
- (nonnull NSString *)weightName __attribute((ns_returns_retained));
/** Weight in units described by weightName. */
- (double)weight;
/** The locale used for voice instructions. */
- (nonnull NSString *)voiceLocale __attribute((ns_returns_retained));
- (nonnull MBNNRDTRdLegArray *)legs __attribute((ns_returns_retained));
- (nonnull MBNNRDTRdCoordinateArray *)geometry __attribute((ns_returns_retained));
- (nonnull MBNNRDTRdWaypointArray *)waypoints __attribute((ns_returns_retained));

@end
