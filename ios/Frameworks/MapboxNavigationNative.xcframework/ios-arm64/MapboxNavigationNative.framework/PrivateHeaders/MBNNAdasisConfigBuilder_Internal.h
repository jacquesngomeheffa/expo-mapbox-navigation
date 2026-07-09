// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNAdasisConfigBuilder.h>

@interface MBNNAdasisConfigBuilder ()
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Builds adasis v2 configuration from JSON.
 * Example JSON:
 * ```
 * {
 * "dataSending": {
 * "outputBinaryFormat": "ADASISv2BE",
 * "enableRetransmission": false,
 * "messageIntervalMs": 100,
 * "messagesInPackage": 4,
 * "metadataCycleSeconds": 5,
 * "retransmissionMeters": 100,
 * "treeTrailingLength": 300,
 * "updatesEnabled": true,
 * "continuePathOnOffsetOverflow": true,
 * "setRouteIsReset": true
 * },
 * "pathOptions": {
 * "profilelong": {
 * "options": {
 * "branchRadiusMeters": 50,
 * "enabled": true,
 * "radiusMeters": 2000
 * },
 * "types": {
 * "customEvents": {
 * "trafficEvent": {
 * "profileType": 19,
 * "trafficEventIds": {
 * "teUnknownId": 0,
 * "teFlowUnknownId": 1,
 * "teFlowSmoothId": 2,
 * "teFlowAmbleId": 3,
 * "teFlowCongestionId": 4,
 * "teFlowCloggedId": 5,
 * "teFlowNoTrafficFlowId": 6,
 * "teAccidentGeneralTrafficAccidentId": 7,
 * "teAccidentSeriousTrafficAccidentId": 8,
 * "teAccidentBreakdownCarId": 9,
 * "teConstructionRoadWorkId": 11,
 * "teConstructionNotRecommendedId": 13,
 * "teControlTrafficControlId": 14,
 * "teControlRoadClosedId": 15,
 * "teControlExitRampClosedId": 16,
 * "teControlOnrampClosedId": 17,
 * "tePavementWaterAccumulationOnRoadId": 56,
 * "tePavementSnowOnRoadId": 57,
 * "tePavementThinIceOnRoadId": 58,
 * "tePavementRoadSubsidenceId": 59,
 * "tePavementObstaclesOnRoadId": 60,
 * "tePavementRoadFireId": 61,
 * "tePavementSlipperyRoadId": 62,
 * "tePavementOilOnRoadId": 63,
 * "teActivityExpoId": 68,
 * "teActivityMajorNationalEventsId": 69,
 * "teActivityGatheringId": 70,
 * "teActivityLargeMeetingId": 71,
 * "teActivitySportsActivitiesId": 72,
 * "teActivityLiteraryActivitiesId": 73,
 * "teActivityHolidaysId": 74,
 * "teDisasterFloodId": 75,
 * "teDisasterEarthquakeId": 76,
 * "teDisasterRockslideId": 77,
 * "teDisasterCollapseId": 78,
 * "teDisasterMudslideId": 79,
 * "tePoliceCatchViolationsId": 80,
 * "tePoliceDrunkDrivingCheckId": 81,
 * "tePolicePoliceEnforcementId": 82,
 * "teOtherAnnouncementId": 83,
 * "teOtherOpenToTrafficId": 84,
 * "teOtherCompleteRemodelId": 85,
 * "teOtherRealRoadConditionsId": 86,
 * "teOtherEmergencyId": 87,
 * "teOtherSubwayIncidentId": 88,
 * "teOtherCustomizedBroadcastId": 89,
 * "teIncidentCtrlGeneralTrafficAccidentsId": 90,
 * "teIncidentCtrlSeriousTrafficAccidentId": 91,
 * "teConstructionCtrlRoadWorksId": 92,
 * "teWeatherCtrlHeavyFogId": 93,
 * "teWeatherCtrlHeavyRainId": 94,
 * "teWeatherCtrlHeavySnowId": 95,
 * "teWeatherCtrlHailId": 96,
 * "tePavementCtrlWaterAccumulationId": 97,
 * "tePavementCtrlSnowyRoadsId": 98,
 * "tePavementCtrlThinIceOnRoadId": 99,
 * "tePavementCtrlRoadSubsidenceId": 100,
 * "tePavementCtrlObstaclesOnRoadId": 101,
 * "tePavementCtrlSevereFiresId": 102,
 * "teActivitiesCtrlFairId": 103,
 * "teActivitiesCtrlNationalEventsId": 104,
 * "teActivitiesCtrlRallyId": 105,
 * "teActivitiesCtrlLargeMeetingsId": 106,
 * "teActivitiesCtrlSportsEventsId": 107,
 * "teActivitiesCtrlCulturalEventsId": 108,
 * "teActivitiesCtrlHolidaysId": 109,
 * "teDisasterCtrlFloodingId": 110,
 * "teDisasterCtrlMudslidesId": 114
 * },
 * "radius": 1000
 * },
 * "weather": {
 * "profileType": 20
 * }
 * },
 * "lonLat": {
 * "enabled": false
 * },
 * "trafficSigns": {
 * "profileType": 8,
 * "trafficSignIds": {
 * "tsDangerId": 0,
 * "tsPassLeftOrRightSideId": 1,
 * "tsPassLeftSideId": 2,
 * "tsPassRightSideId": 3,
 * "tsDomesticAnimalsCrossingId": 4,
 * "tsWildAnimalsCrossingId": 5,
 * "tsRoadWorksId": 6,
 * "tsResidentialAreaId": 7,
 * "tsEndOfResidentialAreaId": 8,
 * "tsRightBendId": 9,
 * "tsLeftBendId": 10,
 * "tsDoubleBendRightFirstId": 13,
 * "tsDoubleBendLeftFirstId": 14,
 * "tsCurvyRoadId": 17,
 * "tsOvertakingByGoodsVehiclesProhibitedId": 20,
 * "tsEndOfProhibitionOnOvertakingForGoodsVehiclesId": 21,
 * "tsDangerousIntersectionId": 22,
 * "tsTunnelId": 24,
 * "tsFerryTerminalId": 25,
 * "tsNarrowBridgeId": 26,
 * "tsHumpbackBridgeBridgeId": 28,
 * "tsRiverBankId": 29,
 * "tsRiverBankLeftId": 30,
 * "tsYieldId": 32,
 * "tsStopId": 33,
 * "tsPriorityRoadId": 34,
 * "tsIntersectionId": 35,
 * "tsIntersectionWithMinorRoadId": 36,
 * "tsIntersectionWithPriorityToTheRightId": 37,
 * "tsDirectionToTheRightId": 38,
 * "tsDirectionToTheLeftId": 39,
 * "tsCarriagewayNarrowsId": 40,
 * "tsCarriagewayNarrowsRightId": 41,
 * "tsCarriagewayNarrowsLeftId": 42,
 * "tsLaneMergeLeftId": 43,
 * "tsLaneMergeRightId": 44,
 * "tsLaneMergeCenterId": 45,
 * "tsOvertakingProhibitedId": 46,
 * "tsEndOfProhibitionOnOvertakingId": 47,
 * "tsProtectiveOvertakingId": 49,
 * "tsPedestriansId": 50,
 * "tsPedestrianCrossingId": 51,
 * "tsChildrenId": 52,
 * "tsSchoolZoneId": 53,
 * "tsCyclistsId": 54,
 * "tsTwoWayTrafficId": 55,
 * "tsRailwayCrossingWithGatesId": 56,
 * "tsRailwayCrossingWithoutGatesId": 57,
 * "tsRailwayCrossingId": 58,
 * "tsTramwayId": 59,
 * "tsFallingRocksId": 60,
 * "tsFallingRocksLeftId": 61,
 * "tsFallingRocksRightId": 62,
 * "tsSteepDropLeftId": 63,
 * "tsSteepDropRightId": 64,
 * "tsVariableSignMechanicElementsId": 65,
 * "tsSlipperyRoadId": 66,
 * "tsSteepAscentId": 67,
 * "tsSteepDescentId": 68,
 * "tsUnevenRoadId": 69,
 * "tsHumpId": 70,
 * "tsDipId": 71,
 * "tsRoadFloodsId": 72,
 * "tsIcyRoadId": 73,
 * "tsSideWindsId": 74,
 * "tsTrafficCongestionId": 75,
 * "tsHighAccidentAreaId": 76,
 * "tsVariableSignLightElementsId": 80,
 * "tsPriorityOverOncomingTrafficId": 81,
 * "tsPriorityForOncomingTrafficId": 82,
 * "tsSpeedLimitId": 87,
 * "tsTollBoothId": 90,
 * "tsRoadCamSpeedIntervalEndId": 237,
 * "tsRoadCamSpeedIntervalStartId": 238,
 * "tsRoadCamSpeedIntervalId": 239,
 * "tsRoadCamLaneNonMotorizedId": 240,
 * "tsRoadCamLaneEmergencyId": 241,
 * "tsRoadCamLaneBusId": 242,
 * "tsRoadCamViolationId": 243,
 * "tsRoadCamRedLightId": 244,
 * "tsRoadCamSurveillanceId": 249,
 * "tsRoadCamSpeedCurrentSpeedId": 250,
 * "tsRailroadCrossingId": 251,
 * "tsZebraId": 252,
 * "tsSpeedBumpId": 253,
 * "tsTrafficLightId": 254
 * },
 * "customConditionForTrailer": 14
 * }
 * }
 * },
 * "profileshort": {
 * "bandwidthEnabled": true,
 * "minDistanceBetweenCurvatures": 0,
 * "options": {
 * "branchRadiusMeters": 50,
 * "enabled": true,
 * "radiusMeters": 2000
 * },
 * "types": {
 * "curvature": true,
 * "headingChange": true,
 * "historyAverageSpeed": true,
 * "roadCondition": true,
 * "slopeLinear": false,
 * "slopeStep": true,
 * "variableSpeedSign": true
 * }
 * },
 * "segment": {
 * "options": {
 * "branchRadiusMeters": 50,
 * "enabled": true,
 * "radiusMeters": 2000
 * }
 * },
 * "stub": {
 * "options": {
 * "branchRadiusMeters": 50,
 * "enabled": true,
 * "radiusMeters": 2000
 * }
 * }
 * }
 * }
 * ```
 */
+ (nonnull MBXExpected<MBNNAdasisConfig *, NSString *> *)fromJsonForJson:(nonnull NSString *)json __attribute((ns_returns_retained));
@end
