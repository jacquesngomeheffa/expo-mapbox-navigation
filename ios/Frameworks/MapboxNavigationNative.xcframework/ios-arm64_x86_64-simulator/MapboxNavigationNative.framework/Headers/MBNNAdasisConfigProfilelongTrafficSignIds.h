// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * PROFILE LONG message traffic sign options
 * Enable / disable each of supported signs type
 * Only values from range 0..254 are valid, -1 means disabled TrafficSign type
 * Default values are mostly from ADASISv2 specification
 */
NS_SWIFT_NAME(AdasisConfigProfilelongTrafficSignIds)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigProfilelongTrafficSignIds : NSObject

- (nonnull instancetype)init;

- (nonnull instancetype)initWithTsDangerId:(int32_t)tsDangerId
                   tsPassLeftOrRightSideId:(int32_t)tsPassLeftOrRightSideId
                          tsPassLeftSideId:(int32_t)tsPassLeftSideId
                         tsPassRightSideId:(int32_t)tsPassRightSideId
               tsDomesticAnimalsCrossingId:(int32_t)tsDomesticAnimalsCrossingId
                   tsWildAnimalsCrossingId:(int32_t)tsWildAnimalsCrossingId
                             tsRoadWorksId:(int32_t)tsRoadWorksId
                       tsResidentialAreaId:(int32_t)tsResidentialAreaId
                  tsEndOfResidentialAreaId:(int32_t)tsEndOfResidentialAreaId
                             tsRightBendId:(int32_t)tsRightBendId
                              tsLeftBendId:(int32_t)tsLeftBendId
                  tsDoubleBendRightFirstId:(int32_t)tsDoubleBendRightFirstId
                   tsDoubleBendLeftFirstId:(int32_t)tsDoubleBendLeftFirstId
                             tsCurvyRoadId:(int32_t)tsCurvyRoadId
   tsOvertakingByGoodsVehiclesProhibitedId:(int32_t)tsOvertakingByGoodsVehiclesProhibitedId
tsEndOfProhibitionOnOvertakingForGoodsVehiclesId:(int32_t)tsEndOfProhibitionOnOvertakingForGoodsVehiclesId
                 tsDangerousIntersectionId:(int32_t)tsDangerousIntersectionId
                                tsTunnelId:(int32_t)tsTunnelId
                         tsFerryTerminalId:(int32_t)tsFerryTerminalId
                          tsNarrowBridgeId:(int32_t)tsNarrowBridgeId
                  tsHumpbackBridgeBridgeId:(int32_t)tsHumpbackBridgeBridgeId
                             tsRiverBankId:(int32_t)tsRiverBankId
                         tsRiverBankLeftId:(int32_t)tsRiverBankLeftId
                                 tsYieldId:(int32_t)tsYieldId
                                  tsStopId:(int32_t)tsStopId
                          tsPriorityRoadId:(int32_t)tsPriorityRoadId
                          tsIntersectionId:(int32_t)tsIntersectionId
             tsIntersectionWithMinorRoadId:(int32_t)tsIntersectionWithMinorRoadId
    tsIntersectionWithPriorityToTheRightId:(int32_t)tsIntersectionWithPriorityToTheRightId
                   tsDirectionToTheRightId:(int32_t)tsDirectionToTheRightId
                    tsDirectionToTheLeftId:(int32_t)tsDirectionToTheLeftId
                    tsCarriagewayNarrowsId:(int32_t)tsCarriagewayNarrowsId
               tsCarriagewayNarrowsRightId:(int32_t)tsCarriagewayNarrowsRightId
                tsCarriagewayNarrowsLeftId:(int32_t)tsCarriagewayNarrowsLeftId
                         tsLaneMergeLeftId:(int32_t)tsLaneMergeLeftId
                        tsLaneMergeRightId:(int32_t)tsLaneMergeRightId
                       tsLaneMergeCenterId:(int32_t)tsLaneMergeCenterId
                  tsOvertakingProhibitedId:(int32_t)tsOvertakingProhibitedId
          tsEndOfProhibitionOnOvertakingId:(int32_t)tsEndOfProhibitionOnOvertakingId
                  tsProtectiveOvertakingId:(int32_t)tsProtectiveOvertakingId
                           tsPedestriansId:(int32_t)tsPedestriansId
                    tsPedestrianCrossingId:(int32_t)tsPedestrianCrossingId
                              tsChildrenId:(int32_t)tsChildrenId
                            tsSchoolZoneId:(int32_t)tsSchoolZoneId
                              tsCyclistsId:(int32_t)tsCyclistsId
                         tsTwoWayTrafficId:(int32_t)tsTwoWayTrafficId
              tsRailwayCrossingWithGatesId:(int32_t)tsRailwayCrossingWithGatesId
           tsRailwayCrossingWithoutGatesId:(int32_t)tsRailwayCrossingWithoutGatesId
                       tsRailwayCrossingId:(int32_t)tsRailwayCrossingId
                               tsTramwayId:(int32_t)tsTramwayId
                          tsFallingRocksId:(int32_t)tsFallingRocksId
                      tsFallingRocksLeftId:(int32_t)tsFallingRocksLeftId
                     tsFallingRocksRightId:(int32_t)tsFallingRocksRightId
                         tsSteepDropLeftId:(int32_t)tsSteepDropLeftId
                        tsSteepDropRightId:(int32_t)tsSteepDropRightId
          tsVariableSignMechanicElementsId:(int32_t)tsVariableSignMechanicElementsId
                          tsSlipperyRoadId:(int32_t)tsSlipperyRoadId
                           tsSteepAscentId:(int32_t)tsSteepAscentId
                          tsSteepDescentId:(int32_t)tsSteepDescentId
                            tsUnevenRoadId:(int32_t)tsUnevenRoadId
                                  tsHumpId:(int32_t)tsHumpId
                                   tsDipId:(int32_t)tsDipId
                            tsRoadFloodsId:(int32_t)tsRoadFloodsId
                               tsIcyRoadId:(int32_t)tsIcyRoadId
                             tsSideWindsId:(int32_t)tsSideWindsId
                     tsTrafficCongestionId:(int32_t)tsTrafficCongestionId
                      tsHighAccidentAreaId:(int32_t)tsHighAccidentAreaId
             tsVariableSignLightElementsId:(int32_t)tsVariableSignLightElementsId
           tsPriorityOverOncomingTrafficId:(int32_t)tsPriorityOverOncomingTrafficId
            tsPriorityForOncomingTrafficId:(int32_t)tsPriorityForOncomingTrafficId
                            tsSpeedLimitId:(int32_t)tsSpeedLimitId
                             tsTollBoothId:(int32_t)tsTollBoothId
               tsRoadCamSpeedIntervalEndId:(int32_t)tsRoadCamSpeedIntervalEndId
             tsRoadCamSpeedIntervalStartId:(int32_t)tsRoadCamSpeedIntervalStartId
                  tsRoadCamSpeedIntervalId:(int32_t)tsRoadCamSpeedIntervalId
               tsRoadCamLaneNonMotorizedId:(int32_t)tsRoadCamLaneNonMotorizedId
                  tsRoadCamLaneEmergencyId:(int32_t)tsRoadCamLaneEmergencyId
                        tsRoadCamLaneBusId:(int32_t)tsRoadCamLaneBusId
                      tsRoadCamViolationId:(int32_t)tsRoadCamViolationId
                       tsRoadCamRedLightId:(int32_t)tsRoadCamRedLightId
                   tsRoadCamSurveillanceId:(int32_t)tsRoadCamSurveillanceId
              tsRoadCamSpeedCurrentSpeedId:(int32_t)tsRoadCamSpeedCurrentSpeedId
                      tsRailroadCrossingId:(int32_t)tsRailroadCrossingId
                                 tsZebraId:(int32_t)tsZebraId
                             tsSpeedBumpId:(int32_t)tsSpeedBumpId
                          tsTrafficLightId:(int32_t)tsTrafficLightId;

@property (nonatomic, readonly) int32_t tsDangerId;
@property (nonatomic, readonly) int32_t tsPassLeftOrRightSideId;
@property (nonatomic, readonly) int32_t tsPassLeftSideId;
@property (nonatomic, readonly) int32_t tsPassRightSideId;
@property (nonatomic, readonly) int32_t tsDomesticAnimalsCrossingId;
@property (nonatomic, readonly) int32_t tsWildAnimalsCrossingId;
@property (nonatomic, readonly) int32_t tsRoadWorksId;
@property (nonatomic, readonly) int32_t tsResidentialAreaId;
@property (nonatomic, readonly) int32_t tsEndOfResidentialAreaId;
@property (nonatomic, readonly) int32_t tsRightBendId;
@property (nonatomic, readonly) int32_t tsLeftBendId;
@property (nonatomic, readonly) int32_t tsDoubleBendRightFirstId;
@property (nonatomic, readonly) int32_t tsDoubleBendLeftFirstId;
@property (nonatomic, readonly) int32_t tsCurvyRoadId;
@property (nonatomic, readonly) int32_t tsOvertakingByGoodsVehiclesProhibitedId;
@property (nonatomic, readonly) int32_t tsEndOfProhibitionOnOvertakingForGoodsVehiclesId;
@property (nonatomic, readonly) int32_t tsDangerousIntersectionId;
@property (nonatomic, readonly) int32_t tsTunnelId;
@property (nonatomic, readonly) int32_t tsFerryTerminalId;
@property (nonatomic, readonly) int32_t tsNarrowBridgeId;
@property (nonatomic, readonly) int32_t tsHumpbackBridgeBridgeId;
@property (nonatomic, readonly) int32_t tsRiverBankId;
@property (nonatomic, readonly) int32_t tsRiverBankLeftId;
@property (nonatomic, readonly) int32_t tsYieldId;
@property (nonatomic, readonly) int32_t tsStopId;
@property (nonatomic, readonly) int32_t tsPriorityRoadId;
@property (nonatomic, readonly) int32_t tsIntersectionId;
@property (nonatomic, readonly) int32_t tsIntersectionWithMinorRoadId;
@property (nonatomic, readonly) int32_t tsIntersectionWithPriorityToTheRightId;
@property (nonatomic, readonly) int32_t tsDirectionToTheRightId;
@property (nonatomic, readonly) int32_t tsDirectionToTheLeftId;
@property (nonatomic, readonly) int32_t tsCarriagewayNarrowsId;
@property (nonatomic, readonly) int32_t tsCarriagewayNarrowsRightId;
@property (nonatomic, readonly) int32_t tsCarriagewayNarrowsLeftId;
@property (nonatomic, readonly) int32_t tsLaneMergeLeftId;
@property (nonatomic, readonly) int32_t tsLaneMergeRightId;
@property (nonatomic, readonly) int32_t tsLaneMergeCenterId;
@property (nonatomic, readonly) int32_t tsOvertakingProhibitedId;
@property (nonatomic, readonly) int32_t tsEndOfProhibitionOnOvertakingId;
@property (nonatomic, readonly) int32_t tsProtectiveOvertakingId;
@property (nonatomic, readonly) int32_t tsPedestriansId;
@property (nonatomic, readonly) int32_t tsPedestrianCrossingId;
@property (nonatomic, readonly) int32_t tsChildrenId;
@property (nonatomic, readonly) int32_t tsSchoolZoneId;
@property (nonatomic, readonly) int32_t tsCyclistsId;
@property (nonatomic, readonly) int32_t tsTwoWayTrafficId;
@property (nonatomic, readonly) int32_t tsRailwayCrossingWithGatesId;
@property (nonatomic, readonly) int32_t tsRailwayCrossingWithoutGatesId;
@property (nonatomic, readonly) int32_t tsRailwayCrossingId;
@property (nonatomic, readonly) int32_t tsTramwayId;
@property (nonatomic, readonly) int32_t tsFallingRocksId;
@property (nonatomic, readonly) int32_t tsFallingRocksLeftId;
@property (nonatomic, readonly) int32_t tsFallingRocksRightId;
@property (nonatomic, readonly) int32_t tsSteepDropLeftId;
@property (nonatomic, readonly) int32_t tsSteepDropRightId;
@property (nonatomic, readonly) int32_t tsVariableSignMechanicElementsId;
@property (nonatomic, readonly) int32_t tsSlipperyRoadId;
@property (nonatomic, readonly) int32_t tsSteepAscentId;
@property (nonatomic, readonly) int32_t tsSteepDescentId;
@property (nonatomic, readonly) int32_t tsUnevenRoadId;
@property (nonatomic, readonly) int32_t tsHumpId;
@property (nonatomic, readonly) int32_t tsDipId;
@property (nonatomic, readonly) int32_t tsRoadFloodsId;
@property (nonatomic, readonly) int32_t tsIcyRoadId;
@property (nonatomic, readonly) int32_t tsSideWindsId;
@property (nonatomic, readonly) int32_t tsTrafficCongestionId;
@property (nonatomic, readonly) int32_t tsHighAccidentAreaId;
@property (nonatomic, readonly) int32_t tsVariableSignLightElementsId;
@property (nonatomic, readonly) int32_t tsPriorityOverOncomingTrafficId;
@property (nonatomic, readonly) int32_t tsPriorityForOncomingTrafficId;
@property (nonatomic, readonly) int32_t tsSpeedLimitId;
@property (nonatomic, readonly) int32_t tsTollBoothId;
@property (nonatomic, readonly) int32_t tsRoadCamSpeedIntervalEndId;
@property (nonatomic, readonly) int32_t tsRoadCamSpeedIntervalStartId;
@property (nonatomic, readonly) int32_t tsRoadCamSpeedIntervalId;
@property (nonatomic, readonly) int32_t tsRoadCamLaneNonMotorizedId;
@property (nonatomic, readonly) int32_t tsRoadCamLaneEmergencyId;
@property (nonatomic, readonly) int32_t tsRoadCamLaneBusId;
@property (nonatomic, readonly) int32_t tsRoadCamViolationId;
@property (nonatomic, readonly) int32_t tsRoadCamRedLightId;
@property (nonatomic, readonly) int32_t tsRoadCamSurveillanceId;
@property (nonatomic, readonly) int32_t tsRoadCamSpeedCurrentSpeedId;
@property (nonatomic, readonly) int32_t tsRailroadCrossingId;
@property (nonatomic, readonly) int32_t tsZebraId;
@property (nonatomic, readonly) int32_t tsSpeedBumpId;
@property (nonatomic, readonly) int32_t tsTrafficLightId;

- (BOOL)isEqualToAdasisConfigProfilelongTrafficSignIds:(nonnull MBNNAdasisConfigProfilelongTrafficSignIds *)other;

@end
