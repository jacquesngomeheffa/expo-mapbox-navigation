// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPoint3d;
typedef NS_ENUM(NSInteger, MBNNLaneChangeDirection);
typedef NS_ENUM(NSInteger, MBNNLaneChangeState);

NS_SWIFT_NAME(LaneChangeAssistData)
__attribute__((visibility ("default")))
@interface MBNNLaneChangeAssistData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithLaneChangeDirection:(MBNNLaneChangeDirection)laneChangeDirection
                                    laneChangeState:(MBNNLaneChangeState)laneChangeState
                                   laneChangeOffset:(nullable MBNNPoint3d *)laneChangeOffset
                      monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Direction of the lane change maneuver.
 */
@property (nonatomic, readonly) MBNNLaneChangeDirection laneChangeDirection;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Current state of the lane change maneuver.
 */
@property (nonatomic, readonly) MBNNLaneChangeState laneChangeState;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Optional offset in meters relative to ego vehicle.
 * The offset acts as a hint towards the direction of the planned lane change.
 * The x coordinate always shifts the lane change indicator towards the target.
 * The y coordinate is used conditionally depending on the context of the lane change.
 * Defaults to 0,0 if not provided and the lane change assist is displayed on the closest lane towards the direction.
 * The coordinate system must adhere ISO 8855.
 */
@property (nonatomic, readonly, nullable) MBNNPoint3d *laneChangeOffset;

/**
 * Time when the data was created. The timestamp must be monotonic.
 * The timestamp should be captured as close as possible to the moment the data is produced.
 */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToLaneChangeAssistData:(nonnull MBNNLaneChangeAssistData *)other;

@end
