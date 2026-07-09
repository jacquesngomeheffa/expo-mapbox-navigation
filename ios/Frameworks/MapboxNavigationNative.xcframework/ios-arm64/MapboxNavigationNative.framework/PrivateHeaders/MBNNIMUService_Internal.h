// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <MapboxNavigationNative/MBNNAltimeterUpdateCallback_Internal.h>
#import <MapboxNavigationNative/MBNNCompassUpdateCallback_Internal.h>
#import <MapboxNavigationNative/MBNNIMUUpdateCallback_Internal.h>

NS_SWIFT_NAME(IMUService)
__attribute__((deprecated))
@protocol MBNNIMUService
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Registers new compass update callback
 * @param callback would be called each time when new compass measurement is available
 * @return id of callback which can be used afterwards as `unregisterCallback` argument
 */
- (int32_t)registerCompassUpdateCallbackForCallback:(nonnull MBNNCompassUpdateCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Registers new IMU update callback
 * @param callback would be called each time when new IMU measurement is available
 * @return id of callback which can be used afterwards as `unregisterCallback` argument
 */
- (int32_t)registerIMUUpdateCallbackForCallback:(nonnull MBNNIMUUpdateCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Registers new altimeter update callback
 * @param callback would be called each time when new IMU measurement is available
 * @return id of callback which can be used afterwards as `unregisterCallback` argument
 */
- (int32_t)registerAltimeterUpdateCallbackForCallback:(nonnull MBNNAltimeterUpdateCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Unregisters callback
 * @param id returned by any of `register*Callback` methods
 */
- (void)unregisterCallbackForId:(int32_t)id_;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * updates current user location, needed to calculate current magnetic declination in order to calculate true heading on Android
 * @param coordinate
 * @param altitude in meters
 */
- (void)updateForCoordinate:(CLLocationCoordinate2D)coordinate
                   altitude:(nullable NSNumber *)altitude;
@end
