// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

NS_SWIFT_NAME(LaneGroupData)
__attribute__((visibility ("default")))
@interface MBNNLaneGroupData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(uint64_t)id_
                           laneIds:(nonnull NSArray<NSNumber *> *)laneIds
                            length:(double)length
                        isJunction:(BOOL)isJunction
              outboundLaneGroupIds:(nonnull NSArray<NSNumber *> *)outboundLaneGroupIds
              incomingLaneGroupIds:(nonnull NSArray<NSNumber *> *)incomingLaneGroupIds
                            zLevel:(int64_t)zLevel
                       elevationId:(nullable NSNumber *)elevationId;

/** Lane group (aka road center line) id */
@property (nonatomic, readonly) uint64_t id;

/** Lane ids belonging to the group, ordered from the leftmost to rightmost */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *laneIds;

/** Length of road geometry */
@property (nonatomic, readonly) double length;

/** True if the lane group is inside a junction or a crossroad */
@property (nonatomic, readonly) BOOL isJunction;

/** Outbound lane group ids */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *outboundLaneGroupIds;

/** Incoming lane group ids */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *incomingLaneGroupIds;

/** z level of the road relative to the adjacent ones. Might be negative */
@property (nonatomic, readonly) int64_t zLevel;

/** Elevation id that associate this edge with a unique elevation */
@property (nonatomic, readonly, nullable) NSNumber *elevationId;


@end
