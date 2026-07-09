// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNFormOfWay)
{
    MBNNFormOfWayUnknown = 0,
    MBNNFormOfWayFreeway = 1,
    MBNNFormOfWayMultipleCarriageway = 2,
    MBNNFormOfWaySingleCarriageway = 3,
    MBNNFormOfWayRoundaboutCircle = 4,
    MBNNFormOfWayTrafficSquare = 5,
    MBNNFormOfWaySlipRoad = 6,
    MBNNFormOfWayParallelRoad = 8,
    MBNNFormOfWayRampOnFreeway = 9,
    MBNNFormOfWayRamp = 10,
    MBNNFormOfWayServiceRoad = 11,
    MBNNFormOfWayCarParkEntrance = 12,
    MBNNFormOfWayServiceEntrance = 13,
    MBNNFormOfWayPedestrianZone = 14,
    MBNNFormOfWayNA = 15
} NS_SWIFT_NAME(FormOfWay);

NSString* MBNNFormOfWayToString(MBNNFormOfWay form_of_way);
