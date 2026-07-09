// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNScanResultCallback_Internal.h>

NS_SWIFT_NAME(BLEService)
@protocol MBNNBLEService
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Registers new BLE scan result callback
 * @param callback would be called each time new Bluetooth LE scan result is available
 * @return id of callback which can be used afterwards as `unregisterCallback` argument
 */
- (int32_t)registerScanResultCallbackForCallback:(nonnull MBNNScanResultCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Unregisters callback
 * @param id returned by `registerScanResultCallback` method
 */
- (void)unregisterCallbackForId:(int32_t)id_;
@end
