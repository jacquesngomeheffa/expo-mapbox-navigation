// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * State of Charging.
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNChargingState)
{
    /** No charging required or charging stopped */
    MBNNChargingStateNotCharging,
    /** Arrived at EV charging station in guidance mode, waiting for charging to start */
    MBNNChargingStateAwaitCharging,
    /** Charging started (with startCharging() call) */
    MBNNChargingStateCharging,
    /** ExtraCharging, i.e. charging started and state of charge is enough to go further (or charging is unplanned by the current route) */
    MBNNChargingStateExtraCharging
} NS_SWIFT_NAME(ChargingState);

NSString* MBNNChargingStateToString(MBNNChargingState charging_state);
