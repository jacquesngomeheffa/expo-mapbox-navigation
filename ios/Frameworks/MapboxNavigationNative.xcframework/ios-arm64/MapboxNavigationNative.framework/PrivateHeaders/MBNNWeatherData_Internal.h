// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNWeather);
typedef NS_ENUM(NSInteger, MBNNWeatherDetail);

NS_SWIFT_NAME(WeatherData)
__attribute__((visibility ("default")))
@interface MBNNWeatherData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithConditions:(nonnull NSArray<NSNumber *> *)conditions
                                    detail:(nullable NSNumber *)detail
             monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * one of possible weather values
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *conditions;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * more detailed weather information
 */
@property (nonatomic, readonly, nullable) NSNumber *detail;

/** monotonic timestamp in nanoseconds */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToWeatherData:(nonnull MBNNWeatherData *)other;

@end
