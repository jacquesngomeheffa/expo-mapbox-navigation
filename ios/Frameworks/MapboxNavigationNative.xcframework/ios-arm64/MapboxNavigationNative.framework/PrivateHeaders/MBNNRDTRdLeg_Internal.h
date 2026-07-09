// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@protocol MBNNRDTRdStepArray;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Immutable part of Route leg information.
 */
NS_SWIFT_NAME(RdLeg)
@protocol MBNNRDTRdLeg
/** The distance traveled through the waypoints in meters. */
- (double)distance;
/** The estimated travel time through the waypoints in seconds. */
- (double)duration;
/** Summary of the route. */
- (nonnull NSString *)summary;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Route steps.
 */
- (nonnull id<MBNNRDTRdStepArray>)steps;
@end
