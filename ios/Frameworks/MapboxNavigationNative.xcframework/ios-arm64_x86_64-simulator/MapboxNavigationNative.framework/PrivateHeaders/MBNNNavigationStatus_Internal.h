// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNNavigationStatus.h>

@class MBNNActiveGuidanceInfo;
@class MBNNBannerInstruction;
@class MBNNCorrectedLocationData;
@class MBNNFixLocation;
@class MBNNHdMatchingResult;
@class MBNNMapMatcherOutput;
@class MBNNRoadName;
@class MBNNRouteIndices;
@class MBNNSpeedLimit;
@class MBNNTurnLane;
@class MBNNUpcomingRouteAlertUpdate;
@class MBNNVoiceInstruction;
typedef NS_ENUM(NSInteger, MBNNFallbackReason);
typedef NS_ENUM(NSInteger, MBNNOffRoadStateProvider);
typedef NS_ENUM(NSInteger, MBNNRouteState);

@interface MBNNNavigationStatus ()
- (nonnull instancetype)initWithRouteState:(MBNNRouteState)routeState
                                     stale:(BOOL)stale
                                  location:(nonnull MBNNFixLocation *)location
                                isFallback:(BOOL)isFallback
                            fallbackReason:(MBNNFallbackReason)fallbackReason
                                  inTunnel:(BOOL)inTunnel
                            inParkingAisle:(BOOL)inParkingAisle
                              inRoundabout:(BOOL)inRoundabout
                                 predicted:(NSTimeInterval)predicted
                                 turnLanes:(nonnull NSArray<MBNNTurnLane *> *)turnLanes
                                     roads:(nonnull NSArray<MBNNRoadName *> *)roads
                       primaryRouteIndices:(nullable MBNNRouteIndices *)primaryRouteIndices
                   alternativeRouteIndices:(nonnull NSArray<MBNNRouteIndices *> *)alternativeRouteIndices
                 locatedAlternativeRouteId:(nullable NSString *)locatedAlternativeRouteId
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
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Current state of the route following (enum)
 */
@property (nonatomic, readonly) MBNNRouteState routeState;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * enhanced location
 */
@property (nonatomic, readonly, nonnull) MBNNFixLocation *location;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Fallback reason.
 * This field can be used to understand what caused switching to fallback.
 * Used as an internal nav-native field to debug fallback issues.
 */
@property (nonatomic, readonly) MBNNFallbackReason fallbackReason;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * List of turn lane objects from the next intersection (see intersectionIndex)
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNTurnLane *> *turnLanes;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * List of known names and shields for the current road.
 * Semantically corresponds to `BannerSection.components`.
 *
 * To retrieve legacy data:
 * - roadName - join `text` field of `roads` from beginning until `roads` end or `text == "/"`
 * - imageBaseUrl - use the first non-empty `imageBaseUrl` in `roads`
 * - shieldName - get the last path component of `imageBaseUrl`
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRoadName *> *roads;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Position on current primary route, nullopt if no route set
 */
@property (nonatomic, readonly, nullable) MBNNRouteIndices *primaryRouteIndices;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * List of route indices for each alternative route.
 * Do not contain data for primary route because all data related to the primary route is directly in NavigationStatus
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteIndices *> *alternativeRouteIndices;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Contains key points(for example corners) of route shape passed by pack between `getStatus` calls or from last notification of observer.
 * For now can be used in route mode only, always contain at least one point besides the very first `getStatus` call.
 * Only `coordinate`, ``bearing`, `monotonicTimestampNanoseconds` and `time` fields are filled. Last point is always the same as `location`.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNFixLocation *> *keyPoints;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Array of upcoming route alert updates without RouteAlert itself.
 * Use getRouteInfo() to get RouteAlert itself.
 * Doesn't contain route alerts that were already passed by user
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNUpcomingRouteAlertUpdate *> *upcomingRouteAlertUpdates;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Corrected GPS location data.
 */
@property (nonatomic, readonly, nullable) MBNNCorrectedLocationData *correctedLocationData;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * HD MapMatcher output.
 * Empty if HD is disabled, or if HD data (i.e. HD graph) is not available in current location.
 */
@property (nonatomic, readonly, nullable) MBNNHdMatchingResult *hdMatchingResult;

@end
