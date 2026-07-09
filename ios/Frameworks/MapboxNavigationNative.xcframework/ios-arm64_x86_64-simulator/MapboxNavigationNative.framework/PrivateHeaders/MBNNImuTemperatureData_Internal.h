// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Temperature of IMU
 */
NS_SWIFT_NAME(ImuTemperatureData)
__attribute__((visibility ("default")))
@interface MBNNImuTemperatureData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithTemperature:(float)temperature
              monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds;

/** Temperature of IMU (in degrees Celsius) */
@property (nonatomic, readonly) float temperature;

/** Monotonic timestamp in nanoseconds */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToImuTemperatureData:(nonnull MBNNImuTemperatureData *)other;

@end
