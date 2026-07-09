// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNVehicleType);
typedef NS_ENUM(NSInteger, MBNNWeather);

NS_SWIFT_NAME(SpeedLimitRestriction)
__attribute__((visibility ("default")))
@interface MBNNSpeedLimitRestriction : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithWeather:(nonnull NSArray<NSNumber *> *)weather
                      dateTimeCondition:(nonnull NSString *)dateTimeCondition
                           vehicleTypes:(nonnull NSArray<NSNumber *> *)vehicleTypes
                                  lanes:(nonnull NSArray<NSNumber *> *)lanes;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Weather conditions where the speed limit is applied. Empty means all.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *weather;

/** OSM openning_hours format, see https://wiki.openstreetmap.org/wiki/Key:opening_hours */
@property (nonatomic, readonly, nonnull, copy) NSString *dateTimeCondition;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * A list of types of vehicles for that the speed limit is included. Empty means all
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *vehicleTypes;

/** Lane numbers where the speed limit is valid. Empty array means all lanes */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *lanes;


- (BOOL)isEqualToSpeedLimitRestriction:(nonnull MBNNSpeedLimitRestriction *)other;

@end
