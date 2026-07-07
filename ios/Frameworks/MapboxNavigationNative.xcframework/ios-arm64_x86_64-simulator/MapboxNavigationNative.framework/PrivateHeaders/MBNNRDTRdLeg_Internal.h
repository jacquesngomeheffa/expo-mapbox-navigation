// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdStepArray;

NS_SWIFT_NAME(RdLeg)
__attribute__((visibility ("default")))
@interface MBNNRDTRdLeg : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/** The distance traveled through the waypoints in meters. */
- (double)distance;
/** The estimated travel time through the waypoints in seconds. */
- (double)duration;
/** Summary of the route. */
- (nonnull NSString *)summary __attribute((ns_returns_retained));
- (nonnull MBNNRDTRdStepArray *)steps __attribute((ns_returns_retained));

@end
