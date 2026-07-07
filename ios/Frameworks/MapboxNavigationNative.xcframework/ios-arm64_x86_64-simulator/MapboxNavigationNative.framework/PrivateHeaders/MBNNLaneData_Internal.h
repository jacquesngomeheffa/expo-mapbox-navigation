// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNLaneSeparatorType_Internal.h>
#import <MapboxNavigationNative/MBNNLaneType_Internal.h>

NS_SWIFT_NAME(LaneData)
__attribute__((visibility ("default")))
@interface MBNNLaneData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(uint64_t)id_
                       laneGroupId:(uint64_t)laneGroupId
                            length:(double)length
                       rightLaneId:(nullable NSNumber *)rightLaneId
                        leftLaneId:(nullable NSNumber *)leftLaneId
                   outboundLaneIds:(nonnull NSArray<NSNumber *> *)outboundLaneIds
                   incomingLaneIds:(nonnull NSArray<NSNumber *> *)incomingLaneIds
                rightLaneSeparator:(nullable NSNumber *)rightLaneSeparator
                 leftLaneSeparator:(nullable NSNumber *)leftLaneSeparator
                    oncomingLaneId:(nullable NSNumber *)oncomingLaneId
                          laneType:(nullable NSNumber *)laneType;

/** Lane id in HD graph */
@property (nonatomic, readonly) uint64_t id;

/** Lane group id in HD graph */
@property (nonatomic, readonly) uint64_t laneGroupId;

/** Length of lane geometry */
@property (nonatomic, readonly) double length;

/** Right lane id. Empty for rightmost lanes */
@property (nonatomic, readonly, nullable) NSNumber *rightLaneId;

/** Left lane id. Empty for leftmost lanes */
@property (nonatomic, readonly, nullable) NSNumber *leftLaneId;

/** Outbound lane ids */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *outboundLaneIds;

/** Incoming lane ids */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *incomingLaneIds;

@property (nonatomic, readonly, nullable) NSNumber *rightLaneSeparator;
@property (nonatomic, readonly, nullable) NSNumber *leftLaneSeparator;
/** Oncoming lane id, may be filled only for leftmost lanes */
@property (nonatomic, readonly, nullable) NSNumber *oncomingLaneId;

@property (nonatomic, readonly, nullable) NSNumber *laneType;

@end
