// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNRoadMarkingType)
{
    MBNNRoadMarkingTypeUnknown,
    /** Speed limit. The value of the marking is represented in the `extras` field. */
    MBNNRoadMarkingTypeSpeedLimitNumber,
    MBNNRoadMarkingTypeArrowStraight,
    MBNNRoadMarkingTypeArrowLeft,
    MBNNRoadMarkingTypeArrowRight,
    MBNNRoadMarkingTypeArrowLeftRight,
    MBNNRoadMarkingTypeArrowStraightLeft,
    MBNNRoadMarkingTypeArrowStraightRight,
    MBNNRoadMarkingTypeArrowUTurn,
    MBNNRoadMarkingTypeStopLine,
    MBNNRoadMarkingTypeYieldLine,
    MBNNRoadMarkingTypeCrosswalk,
    MBNNRoadMarkingTypeSpeedBump,
    MBNNRoadMarkingTypeBusLaneSymbol,
    MBNNRoadMarkingTypeTaxiLaneSymbol,
    MBNNRoadMarkingTypeBikeLaneSymbol,
    MBNNRoadMarkingTypeParkingSymbol,
    MBNNRoadMarkingTypeTextStop,
    MBNNRoadMarkingTypeTextSlow,
    MBNNRoadMarkingTypeTextBus,
    MBNNRoadMarkingTypeTextTaxi,
    MBNNRoadMarkingTypeTextOnly,
    MBNNRoadMarkingTypeTextSchool
} NS_SWIFT_NAME(RoadMarkingType);

NSString* MBNNRoadMarkingTypeToString(MBNNRoadMarkingType road_marking_type);
