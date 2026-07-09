// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNConnectedLaneSequence;
@class MBNNRouteLaneGroup;

NS_SWIFT_NAME(RouteLaneGuidance)
__attribute__((visibility ("default")))
@interface MBNNRouteLaneGuidance : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithRouteId:(nonnull NSString *)routeId
                        routeLaneGroups:(nonnull NSArray<MBNNRouteLaneGroup *> *)routeLaneGroups
                 connectedLaneSequences:(nonnull NSArray<MBNNConnectedLaneSequence *> *)connectedLaneSequences
          beginDistanceFromStartOfRoute:(double)beginDistanceFromStartOfRoute
            endDistanceFromStartOfRoute:(double)endDistanceFromStartOfRoute;

/** Unique route id. Could be used to get smoothed edge geometries from `LaneGraphAccessor` */
@property (nonatomic, readonly, nonnull, copy) NSString *routeId;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Lane groups matched to the route.
 * The order of lane groups follows the route guidance direction.
 * In other words, it starts from some position before ego and goes onward along the route.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteLaneGroup *> *routeLaneGroups;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * All route lanes are grouped into connected lane sequences.
 * Each lane sequence has its own properties, describing not a singular lane, but the whole sequence.
 * Those sequence properties should be used to decide if we should show this lanes to the user.
 * Example: divergent lane sequence with small length could be hidden to improve UX.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNConnectedLaneSequence *> *connectedLaneSequences;

/**
 * Distance in meters from the start of the route to the begin point of the guidance.
 * Could be negative if the first lane group which starts before the start of the route.
 */
@property (nonatomic, readonly) double beginDistanceFromStartOfRoute;

/** Distance in meters from the start of the route to the end point of the guidance. */
@property (nonatomic, readonly) double endDistanceFromStartOfRoute;


- (BOOL)isEqualToRouteLaneGuidance:(nonnull MBNNRouteLaneGuidance *)other;

@end
