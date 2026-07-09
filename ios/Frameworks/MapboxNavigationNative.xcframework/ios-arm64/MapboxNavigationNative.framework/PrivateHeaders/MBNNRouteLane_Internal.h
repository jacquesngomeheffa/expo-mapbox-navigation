// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteLaneDivergency;

NS_SWIFT_NAME(RouteLane)
__attribute__((visibility ("default")))
@interface MBNNRouteLane : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithLaneId:(uint64_t)laneId
                            divergency:(nullable MBNNRouteLaneDivergency *)divergency
            connectedLaneSequenceIndex:(uint32_t)connectedLaneSequenceIndex;

/** Lane id in HD graph */
@property (nonatomic, readonly) uint64_t laneId;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Holds the information for lanes that are going to lead to the off-route somewhere
 * in the future.
 */
@property (nonatomic, readonly, nullable) MBNNRouteLaneDivergency *divergency;

/** Index of a connected lane sequence in `connectedLaneSequences` to which the current lane belongs */
@property (nonatomic, readonly) uint32_t connectedLaneSequenceIndex;


- (BOOL)isEqualToRouteLane:(nonnull MBNNRouteLane *)other;

@end
