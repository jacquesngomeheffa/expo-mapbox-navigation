// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPoint3d;

NS_SWIFT_NAME(RawGyroscopeData)
__attribute__((visibility ("default")))
@interface MBNNRawGyroscopeData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithRotationRate:(nonnull MBNNPoint3d *)rotationRate
               monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds;

@property (nonatomic, readonly, nonnull) MBNNPoint3d *rotationRate;
/** Monotonic timestamp in nanoseconds */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToRawGyroscopeData:(nonnull MBNNRawGyroscopeData *)other;

@end
