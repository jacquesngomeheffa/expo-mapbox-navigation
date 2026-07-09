// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
@class MBXCoordinate2D;

@class MBNNFixDirectedCoordinate;
@class MBNNLaneData;
@class MBNNLaneGroupData;
@class MBNNLaneWidthPoint;
@class MBNNProjection;

NS_SWIFT_NAME(LaneGraphAccessor)
@protocol MBNNLaneGraphAccessor
/**
 * Returns true if the lane group is virtual.
 *
 * Virtual lane group is a lane group that was dynamically generated to cover a hole in the HD
 * route line through a SD-only segment. The geometry of the virtual lane group is derived from
 * the route geometry.
 */
- (BOOL)isVirtualLaneGroupForLaneGroupId:(uint64_t)laneGroupId;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Gets all data related to the road (aka lane group), except road geometry.
 * Empty if laneGroupId is invalid, or tile with the road is not loaded.
 */
- (nullable MBNNLaneGroupData *)getLaneGroupDataForLaneGroupId:(uint64_t)laneGroupId;
/**
 * Gets lane group geometry (aka road center line).
 * Empty if laneGroupId is invalid, or tile with the lane is not loaded.
 */
- (nullable NSArray<MBXCoordinate2D *> *)getLaneGroupGeometryForLaneGroupId:(uint64_t)laneGroupId;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Gets all data related to the road (aka lane group), except road geometry.
 * Virtual lane group is associated with a specific route, so `routeId` is required to identify it.
 */
- (nullable MBNNLaneGroupData *)getVirtualLaneGroupDataForLaneGroupId:(uint64_t)laneGroupId
                                                              routeId:(nonnull NSString *)routeId;
/**
 * Gets lane group geometry (aka road center line).
 * Virtual lane group is associated with a specific route, so `routeId` is required to identify it.
 */
- (nullable NSArray<MBXCoordinate2D *> *)getVirtualLaneGroupGeometryForLaneGroupId:(uint64_t)laneGroupId
                                                                           routeId:(nonnull NSString *)routeId;
/**
 * Gets lane group's smoothed geometry.
 * Smoothed geometry depends on group sequence we need to smooth, so this method
 * requires `routeId` to identify the sequence.
 * Result is empty if provided route id is unknown, or if it doesn't contain provided `laneGroupId`.
 */
- (nullable NSArray<MBXCoordinate2D *> *)getSmoothedLaneGroupGeometryForLaneGroupId:(uint64_t)laneGroupId
                                                           distanceFromStartOfRoute:(double)distanceFromStartOfRoute
                                                                            routeId:(nonnull NSString *)routeId;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Gets all data related to the lane, except lane geometry.
 * Empty if laneId is invalid, or tile with the lane is not loaded.
 */
- (nullable MBNNLaneData *)getLaneDataForLaneId:(uint64_t)laneId;
/**
 * Gets lane geometry.
 * Empty if laneId is invalid, or tile with the lane is not loaded.
 */
- (nullable NSArray<MBXCoordinate2D *> *)getLaneGeometryForLaneId:(uint64_t)laneId;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Gets lane width points, sorted by percent along the lane geometry.
 * If not empty - it's guaranteed the first point would have `percentAlong = 0.0`,
 * and the last point would have `percentAlong = 1.0`.
 */
- (nullable NSArray<MBNNLaneWidthPoint *> *)getLaneWidthPointsForLaneId:(uint64_t)laneId;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Finds lanes projections near a given coordinate.
 * @coordinate Coordinate to search around.
 * @searchRadius Search radius in meters around the given coordinate.
 *               The distance to all projections will not be greater than this value.
 * @headingThreshold Heading threshold between the given coordinate and lane segment.
 * @segmented Considers each segment of lane geometry as separate projection.
 *
 * @return Returns set of projections.
 */
- (nonnull NSArray<MBNNProjection *> *)findLanesProjectionsForCoordinate:(nonnull MBNNFixDirectedCoordinate *)coordinate
                                                            searchRadius:(double)searchRadius
                                                        headingThreshold:(nullable NSNumber *)headingThreshold
                                                               segmented:(BOOL)segmented;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Finds lane groups projections near a given coordinate.
 * @coordinate Coordinate to search around.
 * @searchRadius Search radius in meters around the given coordinate.
 *               The distance to all projections will not be greater than this value.
 * @headingThreshold Heading threshold between the given coordinate and lane group segment.
 * @segmented Considers each segment of lane group geometry as separate projection.
 *
 * @return Returns set of projections.
 */
- (nonnull NSArray<MBNNProjection *> *)findLaneGroupsProjectionsForCoordinate:(nonnull MBNNFixDirectedCoordinate *)coordinate
                                                                 searchRadius:(double)searchRadius
                                                             headingThreshold:(nullable NSNumber *)headingThreshold
                                                                    segmented:(BOOL)segmented;
@end
