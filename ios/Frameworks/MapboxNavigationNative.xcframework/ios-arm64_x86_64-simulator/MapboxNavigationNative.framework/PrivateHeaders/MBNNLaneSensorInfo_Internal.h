// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

NS_SWIFT_NAME(LaneSensorInfo)
__attribute__((visibility ("default")))
@interface MBNNLaneSensorInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithCurrentLaneIndex:(uint8_t)currentLaneIndex
                                       laneCount:(uint8_t)laneCount
                   monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds;

/** Current lane index, starting from 1 from the road boundary, e.g. for right side traffic 1 means the right lane. 0 - unknown */
@property (nonatomic, readonly) uint8_t currentLaneIndex;

/** Total number of lanes, 0 - unknown */
@property (nonatomic, readonly) uint8_t laneCount;

/** monotonic timestamp in nanoseconds */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToLaneSensorInfo:(nonnull MBNNLaneSensorInfo *)other;

@end
