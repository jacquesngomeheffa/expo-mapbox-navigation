// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNNavigationStatus.h>
#import <MapboxNavigationNative/MBNNRouteState_Internal.h>

@interface MBNNNavigationStatus ()
- (nonnull instancetype)initWithRouteState:(MBNNRouteState)routeState
                 locatedAlternativeRouteId:(nullable NSString *)locatedAlternativeRouteId
                            primaryRouteId:(nullable NSString *)primaryRouteId
                                     stale:(BOOL)stale
                                  location:(nonnull MBNNFixLocation *)location
                                routeIndex:(uint32_t)routeIndex
                                  legIndex:(uint32_t)legIndex
                                 stepIndex:(uint32_t)stepIndex
                                isFallback:(BOOL)isFallback
                                  inTunnel:(BOOL)inTunnel
                            inParkingAisle:(BOOL)inParkingAisle
                              inRoundabout:(BOOL)inRoundabout
                                 predicted:(NSTimeInterval)predicted
                             geometryIndex:(uint32_t)geometryIndex
                                shapeIndex:(uint32_t)shapeIndex
                         intersectionIndex:(uint32_t)intersectionIndex
                                 turnLanes:(nonnull NSArray<MBNNTurnLane *> *)turnLanes
                   alternativeRouteIndices:(nonnull NSArray<MBNNRouteIndices *> *)alternativeRouteIndices
                                     roads:(nonnull NSArray<MBNNRoadName *> *)roads
                          voiceInstruction:(nullable MBNNVoiceInstruction *)voiceInstruction
                         bannerInstruction:(nullable MBNNBannerInstruction *)bannerInstruction
                                speedLimit:(nonnull MBNNSpeedLimit *)speedLimit
                                 keyPoints:(nonnull NSArray<MBNNFixLocation *> *)keyPoints
                          mapMatcherOutput:(nonnull MBNNMapMatcherOutput *)mapMatcherOutput
                              offRoadProba:(float)offRoadProba
                      offRoadStateProvider:(MBNNOffRoadStateProvider)offRoadStateProvider
                        activeGuidanceInfo:(nullable MBNNActiveGuidanceInfo *)activeGuidanceInfo
                 upcomingRouteAlertUpdates:(nonnull NSArray<MBNNUpcomingRouteAlertUpdate *> *)upcomingRouteAlertUpdates
                         nextWaypointIndex:(uint32_t)nextWaypointIndex
                                     layer:(nullable NSNumber *)layer
                       isSyntheticLocation:(BOOL)isSyntheticLocation
                     correctedLocationData:(nullable MBNNCorrectedLocationData *)correctedLocationData
                          hdMatchingResult:(nullable MBNNHdMatchingResult *)hdMatchingResult
                      mapMatchedSystemTime:(nonnull NSDate *)mapMatchedSystemTime
                       isAdasDataAvailable:(nullable NSNumber *)isAdasDataAvailable;
@property (nonatomic, readonly) MBNNRouteState routeState;
@property (nonatomic, readonly, nonnull) MBNNFixLocation *location;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNTurnLane *> *turnLanes;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteIndices *> *alternativeRouteIndices;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRoadName *> *roads;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNFixLocation *> *keyPoints;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNUpcomingRouteAlertUpdate *> *upcomingRouteAlertUpdates;
@property (nonatomic, readonly, nullable) MBNNCorrectedLocationData *correctedLocationData;
@property (nonatomic, readonly, nullable) MBNNHdMatchingResult *hdMatchingResult;
@end
