// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/** Internal nav-native enum used to determine the reason of off-road state */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNHDOffRoadStateProvider)
{
    MBNNHDOffRoadStateProviderUnknown,
    MBNNHDOffRoadStateProviderTransitioning,
    MBNNHDOffRoadStateProviderHMM,
    MBNNHDOffRoadStateProviderTunnel,
    MBNNHDOffRoadStateProviderBadSignal,
    MBNNHDOffRoadStateProviderParkingGarage
} NS_SWIFT_NAME(HDOffRoadStateProvider);

NSString* MBNNHDOffRoadStateProviderToString(MBNNHDOffRoadStateProvider hdoff_road_state_provider);
