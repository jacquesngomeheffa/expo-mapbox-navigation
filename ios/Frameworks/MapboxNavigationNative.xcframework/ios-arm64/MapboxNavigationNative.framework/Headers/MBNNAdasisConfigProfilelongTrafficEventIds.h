// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * PROFILE LONG message traffic event options
 * Enable / disable each of supported event type
 * Only values from range 0..127 are valid, -1 means disabled TrafficEvent type
 */
NS_SWIFT_NAME(AdasisConfigProfilelongTrafficEventIds)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigProfilelongTrafficEventIds : NSObject

- (nonnull instancetype)init;

- (nonnull instancetype)initWithTeUnknownId:(int32_t)teUnknownId
                            teFlowUnknownId:(int32_t)teFlowUnknownId
                             teFlowSmoothId:(int32_t)teFlowSmoothId
                              teFlowAmbleId:(int32_t)teFlowAmbleId
                         teFlowCongestionId:(int32_t)teFlowCongestionId
                            teFlowCloggedId:(int32_t)teFlowCloggedId
                      teFlowNoTrafficFlowId:(int32_t)teFlowNoTrafficFlowId
         teAccidentGeneralTrafficAccidentId:(int32_t)teAccidentGeneralTrafficAccidentId
         teAccidentSeriousTrafficAccidentId:(int32_t)teAccidentSeriousTrafficAccidentId
                   teAccidentBreakdownCarId:(int32_t)teAccidentBreakdownCarId
                   teConstructionRoadWorkId:(int32_t)teConstructionRoadWorkId
             teConstructionNotRecommendedId:(int32_t)teConstructionNotRecommendedId
                  teControlTrafficControlId:(int32_t)teControlTrafficControlId
                      teControlRoadClosedId:(int32_t)teControlRoadClosedId
                  teControlExitRampClosedId:(int32_t)teControlExitRampClosedId
                    teControlOnrampClosedId:(int32_t)teControlOnrampClosedId
        tePavementWaterAccumulationOnRoadId:(int32_t)tePavementWaterAccumulationOnRoadId
                     tePavementSnowOnRoadId:(int32_t)tePavementSnowOnRoadId
                  tePavementThinIceOnRoadId:(int32_t)tePavementThinIceOnRoadId
                 tePavementRoadSubsidenceId:(int32_t)tePavementRoadSubsidenceId
                tePavementObstaclesOnRoadId:(int32_t)tePavementObstaclesOnRoadId
                       tePavementRoadFireId:(int32_t)tePavementRoadFireId
                   tePavementSlipperyRoadId:(int32_t)tePavementSlipperyRoadId
                      tePavementOilOnRoadId:(int32_t)tePavementOilOnRoadId
                           teActivityExpoId:(int32_t)teActivityExpoId
            teActivityMajorNationalEventsId:(int32_t)teActivityMajorNationalEventsId
                      teActivityGatheringId:(int32_t)teActivityGatheringId
                   teActivityLargeMeetingId:(int32_t)teActivityLargeMeetingId
               teActivitySportsActivitiesId:(int32_t)teActivitySportsActivitiesId
             teActivityLiteraryActivitiesId:(int32_t)teActivityLiteraryActivitiesId
                       teActivityHolidaysId:(int32_t)teActivityHolidaysId
                          teDisasterFloodId:(int32_t)teDisasterFloodId
                     teDisasterEarthquakeId:(int32_t)teDisasterEarthquakeId
                      teDisasterRockslideId:(int32_t)teDisasterRockslideId
                       teDisasterCollapseId:(int32_t)teDisasterCollapseId
                       teDisasterMudslideId:(int32_t)teDisasterMudslideId
                  tePoliceCatchViolationsId:(int32_t)tePoliceCatchViolationsId
                tePoliceDrunkDrivingCheckId:(int32_t)tePoliceDrunkDrivingCheckId
                tePolicePoliceEnforcementId:(int32_t)tePolicePoliceEnforcementId
                      teOtherAnnouncementId:(int32_t)teOtherAnnouncementId
                     teOtherOpenToTrafficId:(int32_t)teOtherOpenToTrafficId
                   teOtherCompleteRemodelId:(int32_t)teOtherCompleteRemodelId
                teOtherRealRoadConditionsId:(int32_t)teOtherRealRoadConditionsId
                         teOtherEmergencyId:(int32_t)teOtherEmergencyId
                    teOtherSubwayIncidentId:(int32_t)teOtherSubwayIncidentId
               teOtherCustomizedBroadcastId:(int32_t)teOtherCustomizedBroadcastId
    teIncidentCtrlGeneralTrafficAccidentsId:(int32_t)teIncidentCtrlGeneralTrafficAccidentsId
     teIncidentCtrlSeriousTrafficAccidentId:(int32_t)teIncidentCtrlSeriousTrafficAccidentId
              teConstructionCtrlRoadWorksId:(int32_t)teConstructionCtrlRoadWorksId
                    teWeatherCtrlHeavyFogId:(int32_t)teWeatherCtrlHeavyFogId
                   teWeatherCtrlHeavyRainId:(int32_t)teWeatherCtrlHeavyRainId
                   teWeatherCtrlHeavySnowId:(int32_t)teWeatherCtrlHeavySnowId
                        teWeatherCtrlHailId:(int32_t)teWeatherCtrlHailId
          tePavementCtrlWaterAccumulationId:(int32_t)tePavementCtrlWaterAccumulationId
                 tePavementCtrlSnowyRoadsId:(int32_t)tePavementCtrlSnowyRoadsId
              tePavementCtrlThinIceOnRoadId:(int32_t)tePavementCtrlThinIceOnRoadId
             tePavementCtrlRoadSubsidenceId:(int32_t)tePavementCtrlRoadSubsidenceId
            tePavementCtrlObstaclesOnRoadId:(int32_t)tePavementCtrlObstaclesOnRoadId
                tePavementCtrlSevereFiresId:(int32_t)tePavementCtrlSevereFiresId
                     teActivitiesCtrlFairId:(int32_t)teActivitiesCtrlFairId
           teActivitiesCtrlNationalEventsId:(int32_t)teActivitiesCtrlNationalEventsId
                    teActivitiesCtrlRallyId:(int32_t)teActivitiesCtrlRallyId
            teActivitiesCtrlLargeMeetingsId:(int32_t)teActivitiesCtrlLargeMeetingsId
             teActivitiesCtrlSportsEventsId:(int32_t)teActivitiesCtrlSportsEventsId
           teActivitiesCtrlCulturalEventsId:(int32_t)teActivitiesCtrlCulturalEventsId
                 teActivitiesCtrlHolidaysId:(int32_t)teActivitiesCtrlHolidaysId
                   teDisasterCtrlFloodingId:(int32_t)teDisasterCtrlFloodingId
                  teDisasterCtrlMudslidesId:(int32_t)teDisasterCtrlMudslidesId;

@property (nonatomic, readonly) int32_t teUnknownId;
@property (nonatomic, readonly) int32_t teFlowUnknownId;
@property (nonatomic, readonly) int32_t teFlowSmoothId;
@property (nonatomic, readonly) int32_t teFlowAmbleId;
@property (nonatomic, readonly) int32_t teFlowCongestionId;
@property (nonatomic, readonly) int32_t teFlowCloggedId;
@property (nonatomic, readonly) int32_t teFlowNoTrafficFlowId;
@property (nonatomic, readonly) int32_t teAccidentGeneralTrafficAccidentId;
@property (nonatomic, readonly) int32_t teAccidentSeriousTrafficAccidentId;
@property (nonatomic, readonly) int32_t teAccidentBreakdownCarId;
@property (nonatomic, readonly) int32_t teConstructionRoadWorkId;
@property (nonatomic, readonly) int32_t teConstructionNotRecommendedId;
@property (nonatomic, readonly) int32_t teControlTrafficControlId;
@property (nonatomic, readonly) int32_t teControlRoadClosedId;
@property (nonatomic, readonly) int32_t teControlExitRampClosedId;
@property (nonatomic, readonly) int32_t teControlOnrampClosedId;
@property (nonatomic, readonly) int32_t tePavementWaterAccumulationOnRoadId;
@property (nonatomic, readonly) int32_t tePavementSnowOnRoadId;
@property (nonatomic, readonly) int32_t tePavementThinIceOnRoadId;
@property (nonatomic, readonly) int32_t tePavementRoadSubsidenceId;
@property (nonatomic, readonly) int32_t tePavementObstaclesOnRoadId;
@property (nonatomic, readonly) int32_t tePavementRoadFireId;
@property (nonatomic, readonly) int32_t tePavementSlipperyRoadId;
@property (nonatomic, readonly) int32_t tePavementOilOnRoadId;
@property (nonatomic, readonly) int32_t teActivityExpoId;
@property (nonatomic, readonly) int32_t teActivityMajorNationalEventsId;
@property (nonatomic, readonly) int32_t teActivityGatheringId;
@property (nonatomic, readonly) int32_t teActivityLargeMeetingId;
@property (nonatomic, readonly) int32_t teActivitySportsActivitiesId;
@property (nonatomic, readonly) int32_t teActivityLiteraryActivitiesId;
@property (nonatomic, readonly) int32_t teActivityHolidaysId;
@property (nonatomic, readonly) int32_t teDisasterFloodId;
@property (nonatomic, readonly) int32_t teDisasterEarthquakeId;
@property (nonatomic, readonly) int32_t teDisasterRockslideId;
@property (nonatomic, readonly) int32_t teDisasterCollapseId;
@property (nonatomic, readonly) int32_t teDisasterMudslideId;
@property (nonatomic, readonly) int32_t tePoliceCatchViolationsId;
@property (nonatomic, readonly) int32_t tePoliceDrunkDrivingCheckId;
@property (nonatomic, readonly) int32_t tePolicePoliceEnforcementId;
@property (nonatomic, readonly) int32_t teOtherAnnouncementId;
@property (nonatomic, readonly) int32_t teOtherOpenToTrafficId;
@property (nonatomic, readonly) int32_t teOtherCompleteRemodelId;
@property (nonatomic, readonly) int32_t teOtherRealRoadConditionsId;
@property (nonatomic, readonly) int32_t teOtherEmergencyId;
@property (nonatomic, readonly) int32_t teOtherSubwayIncidentId;
@property (nonatomic, readonly) int32_t teOtherCustomizedBroadcastId;
@property (nonatomic, readonly) int32_t teIncidentCtrlGeneralTrafficAccidentsId;
@property (nonatomic, readonly) int32_t teIncidentCtrlSeriousTrafficAccidentId;
@property (nonatomic, readonly) int32_t teConstructionCtrlRoadWorksId;
@property (nonatomic, readonly) int32_t teWeatherCtrlHeavyFogId;
@property (nonatomic, readonly) int32_t teWeatherCtrlHeavyRainId;
@property (nonatomic, readonly) int32_t teWeatherCtrlHeavySnowId;
@property (nonatomic, readonly) int32_t teWeatherCtrlHailId;
@property (nonatomic, readonly) int32_t tePavementCtrlWaterAccumulationId;
@property (nonatomic, readonly) int32_t tePavementCtrlSnowyRoadsId;
@property (nonatomic, readonly) int32_t tePavementCtrlThinIceOnRoadId;
@property (nonatomic, readonly) int32_t tePavementCtrlRoadSubsidenceId;
@property (nonatomic, readonly) int32_t tePavementCtrlObstaclesOnRoadId;
@property (nonatomic, readonly) int32_t tePavementCtrlSevereFiresId;
@property (nonatomic, readonly) int32_t teActivitiesCtrlFairId;
@property (nonatomic, readonly) int32_t teActivitiesCtrlNationalEventsId;
@property (nonatomic, readonly) int32_t teActivitiesCtrlRallyId;
@property (nonatomic, readonly) int32_t teActivitiesCtrlLargeMeetingsId;
@property (nonatomic, readonly) int32_t teActivitiesCtrlSportsEventsId;
@property (nonatomic, readonly) int32_t teActivitiesCtrlCulturalEventsId;
@property (nonatomic, readonly) int32_t teActivitiesCtrlHolidaysId;
@property (nonatomic, readonly) int32_t teDisasterCtrlFloodingId;
@property (nonatomic, readonly) int32_t teDisasterCtrlMudslidesId;

- (BOOL)isEqualToAdasisConfigProfilelongTrafficEventIds:(nonnull MBNNAdasisConfigProfilelongTrafficEventIds *)other;

@end
