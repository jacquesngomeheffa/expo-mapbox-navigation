// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNVehicleType);
typedef NS_ENUM(NSInteger, MBNNWeather);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Describes current conditions to be applied to find better suitable speed limit
 * Current time and locale is taken from the system
 */
NS_SWIFT_NAME(SpeedLimitCondition)
__attribute__((visibility ("default")))
@interface MBNNSpeedLimitCondition : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithWeather:(nonnull NSArray<NSNumber *> *)weather
                                   lane:(uint8_t)lane
                            vehicleType:(MBNNVehicleType)vehicleType;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Current weather
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *weather;

/** Lane number if known or 0. For right-hand traffic 1 is the very right lane and so on */
@property (nonatomic, readonly) uint8_t lane;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Pass vehicle type here
 */
@property (nonatomic, readonly) MBNNVehicleType vehicleType;


- (BOOL)isEqualToSpeedLimitCondition:(nonnull MBNNSpeedLimitCondition *)other;

@end
