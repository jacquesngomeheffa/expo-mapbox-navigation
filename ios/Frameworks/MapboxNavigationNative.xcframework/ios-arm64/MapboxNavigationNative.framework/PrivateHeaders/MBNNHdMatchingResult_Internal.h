// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNCorrectedLocationData;
@class MBNNLaneGroupMatching;
@class MBNNLaneMatching;
@class MBNNRoutesLaneGuidance;
typedef NS_ENUM(NSInteger, MBNNHDOffRoadStateProvider);

NS_SWIFT_NAME(HdMatchingResult)
__attribute__((visibility ("default")))
@interface MBNNHdMatchingResult : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithLaneGroupMatching:(nullable MBNNLaneGroupMatching *)laneGroupMatching
                                     laneMatching:(nullable MBNNLaneMatching *)laneMatching
                                       isTeleport:(BOOL)isTeleport
                                         isOnRoad:(BOOL)isOnRoad
                               offRoadProbability:(float)offRoadProbability
                             offRoadStateProvider:(MBNNHDOffRoadStateProvider)offRoadStateProvider
                               routesLaneGuidance:(nullable MBNNRoutesLaneGuidance *)routesLaneGuidance
                                         inTunnel:(BOOL)inTunnel
                            correctedLocationData:(nullable MBNNCorrectedLocationData *)correctedLocationData
                             totalCandidatesCount:(uint32_t)totalCandidatesCount;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Road-level matching result.
 * Could be empty if we are unsure on which lane group we are or if the off-road was detected.
 */
@property (nonatomic, readonly, nullable) MBNNLaneGroupMatching *laneGroupMatching;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Lane-level matching result.
 * Could be empty if we are unsure on which lane we are or if the off-road was detected.
 * Also could be empty if HD lite mode is forced in the configuration.
 */
@property (nonatomic, readonly, nullable) MBNNLaneMatching *laneMatching;

/**
 * Boolean flag defining if teleport has just occurred,
 * i.e. snapped location has just jumped onto another road (aka lane group)
 */
@property (nonatomic, readonly) BOOL isTeleport;

/**
 * Indicates if we're currently on road (as opposite to off-road).
 * Note that even with laneGroupMatching and laneMatching present, we might
 * still consider the current vehicle's position as off-road due to various factors.
 */
@property (nonatomic, readonly) BOOL isOnRoad;

/**
 * Probability of the vehicle being off-road:
 * 1.0 - vehicle is definitely off-road, 0.0 - vehicle is definitely on-road.
 */
@property (nonatomic, readonly) float offRoadProbability;

/**
 * HD Off road state provider.
 * This field can be used to understand which off-road detector had the greatest impact on determining HD off-road state.
 * Used as internal nav-native field to debug off-road logic.
 */
@property (nonatomic, readonly) MBNNHDOffRoadStateProvider offRoadStateProvider;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Routes lane guidance
 */
@property (nonatomic, readonly, nullable) MBNNRoutesLaneGuidance *routesLaneGuidance;

/** Is current location belongs to tunnel */
@property (nonatomic, readonly) BOOL inTunnel;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Corrected GPS location data.
 */
@property (nonatomic, readonly, nullable) MBNNCorrectedLocationData *correctedLocationData;

/** Total number of MM candidates including the low-probability ones */
@property (nonatomic, readonly) uint32_t totalCandidatesCount;


- (BOOL)isEqualToHdMatchingResult:(nonnull MBNNHdMatchingResult *)other;

@end
