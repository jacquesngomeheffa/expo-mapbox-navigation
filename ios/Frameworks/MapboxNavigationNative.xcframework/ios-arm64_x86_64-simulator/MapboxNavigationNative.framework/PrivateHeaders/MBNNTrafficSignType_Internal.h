// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNTrafficSignType)
{
    MBNNTrafficSignTypeUnknown,
    /** Speed limits. The value of the sign is represented in the `extras` field. */
    MBNNTrafficSignTypeSpeedLimit,
    /** Turn restrictions and directions */
    MBNNTrafficSignTypeArrowLeft,
    MBNNTrafficSignTypeArrowRight,
    MBNNTrafficSignTypeArrowLeftAhead,
    MBNNTrafficSignTypeArrowRightAhead,
    MBNNTrafficSignTypeArrowLeftOrStraight,
    MBNNTrafficSignTypeArrowRightOrStraight,
    MBNNTrafficSignTypeArrowLeftOrRight,
    MBNNTrafficSignTypeArrowStraight,
    MBNNTrafficSignTypeArrowAllDirectionsPermitted,
    MBNNTrafficSignTypeArrowRoundabout,
    MBNNTrafficSignTypeArrowUTurn,
    MBNNTrafficSignTypeArrowKeepLeft,
    MBNNTrafficSignTypeArrowKeepRight,
    MBNNTrafficSignTypeOnewayArrowLeft,
    MBNNTrafficSignTypeOnewayArrowRight,
    MBNNTrafficSignTypeOnewayArrowStraight,
    MBNNTrafficSignTypeNoLeftTurn,
    MBNNTrafficSignTypeNoRightTurn,
    MBNNTrafficSignTypeNoUTurn,
    MBNNTrafficSignTypeNoStraightThrough,
    MBNNTrafficSignTypeNoLeftOrUTurn,
    MBNNTrafficSignTypeNoStraightThroughOrTurnRight,
    /** Prohibitory signs */
    MBNNTrafficSignTypeNoEntrance,
    MBNNTrafficSignTypeNoVehicles,
    MBNNTrafficSignTypeNoOvertaking,
    MBNNTrafficSignTypeNoMotorVehicles,
    MBNNTrafficSignTypeNoHeavyGoodsVehicles,
    MBNNTrafficSignTypeNoMotorcyclesOrMopeds,
    MBNNTrafficSignTypeNoBicycles,
    MBNNTrafficSignTypeNoPedestrians,
    MBNNTrafficSignTypeNoParking,
    MBNNTrafficSignTypeNoStopping,
    MBNNTrafficSignTypeMotorVehiclesOnly,
    MBNNTrafficSignTypeEndProhibition,
    /** Warning signs */
    MBNNTrafficSignTypeWarningPassLeftOrRight,
    MBNNTrafficSignTypeWarningPedestriansCrossing,
    MBNNTrafficSignTypeWarningSpeedBump,
    MBNNTrafficSignTypeWarningSpeedHump,
    MBNNTrafficSignTypeWarningRoundabout,
    MBNNTrafficSignTypeWarningPriority,
    MBNNTrafficSignTypeWarningSideRoadLeft,
    MBNNTrafficSignTypeWarningSideRoadRight,
    MBNNTrafficSignTypeWarningCrossroads,
    MBNNTrafficSignTypeYieldAlert,
    MBNNTrafficSignTypeStopSign,
    MBNNTrafficSignTypeGiveWayToOncomingTraffic,
    MBNNTrafficSignTypePriorityOverOncomingTraffic,
    /** Priority signs */
    MBNNTrafficSignTypePriorityRoad,
    MBNNTrafficSignTypeEndOfPriorityRoad,
    /** Regulatory and information signs */
    MBNNTrafficSignTypeDeadEnd,
    MBNNTrafficSignTypePedestriansCrossing,
    MBNNTrafficSignTypeHospital,
    MBNNTrafficSignTypeParking
} NS_SWIFT_NAME(TrafficSignType);

NSString* MBNNTrafficSignTypeToString(MBNNTrafficSignType traffic_sign_type);
