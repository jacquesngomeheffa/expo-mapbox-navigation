// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNWeatherDetail_Internal.h>
#import <MapboxNavigationNative/MBNNWeather_Internal.h>

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

@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *conditions;
@property (nonatomic, readonly, nullable) NSNumber *detail;
/** monotonic timestamp in nanoseconds */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToWeatherData:(nonnull MBNNWeatherData *)other;

@end
