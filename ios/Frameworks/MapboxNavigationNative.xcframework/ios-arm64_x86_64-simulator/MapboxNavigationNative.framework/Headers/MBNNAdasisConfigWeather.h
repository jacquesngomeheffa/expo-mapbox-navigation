// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

NS_SWIFT_NAME(AdasisConfigWeather)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigWeather : NSObject

- (nonnull instancetype)init;

- (nonnull instancetype)initWithProfileType:(uint8_t)profileType;

/**
 * Set to 0 to disable or use a number in interval [16..31] to send with appropriate sub-type id.
 * See weather values as WeatherDetail in sensor-data.mbg
 */
@property (nonatomic, readonly) uint8_t profileType;


- (BOOL)isEqualToAdasisConfigWeather:(nonnull MBNNAdasisConfigWeather *)other;

@end
