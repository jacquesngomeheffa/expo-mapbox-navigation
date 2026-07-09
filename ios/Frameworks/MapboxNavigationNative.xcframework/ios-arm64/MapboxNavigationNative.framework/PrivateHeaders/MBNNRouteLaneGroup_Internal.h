// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteLane;

NS_SWIFT_NAME(RouteLaneGroup)
__attribute__((visibility ("default")))
@interface MBNNRouteLaneGroup : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithLaneGroupId:(uint64_t)laneGroupId
                                      lanes:(nonnull NSArray<MBNNRouteLane *> *)lanes
                            distanceFromEgo:(nullable NSNumber *)distanceFromEgo
                   distanceFromStartOfRoute:(double)distanceFromStartOfRoute;

/** Lane group id in HD graph. */
@property (nonatomic, readonly) uint64_t laneGroupId;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Lanes relevant to the edge and matched to the route.
 * Properties of the same lane may vary from group to group if the route has loops.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteLane *> *lanes;

/**
 * Distance in meters from ego position to the start of the lane group.
 * Could be negative for lane groups which start before the ego car position.
 * Empty value means the route lane group is not reachable from the current ego position,
 * that may happen when we deviate from the current route.
 */
@property (nonatomic, readonly, nullable) NSNumber *distanceFromEgo;

/**
 * Distance in meters from the start of the route to the start of the lane group.
 * Could be negative for the first lane group which starts before the start of the route.
 */
@property (nonatomic, readonly) double distanceFromStartOfRoute;


- (BOOL)isEqualToRouteLaneGroup:(nonnull MBNNRouteLaneGroup *)other;

@end
