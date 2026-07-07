// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRoadGraphUpdateAvailabilityCallback_Internal.h>
#import <MapboxNavigationNative/MBNNRoadGraphVersionInfoCallback_Internal.h>

@protocol MBXCancelable;

NS_SWIFT_NAME(CacheHandle)
__attribute__((visibility ("default")))
@interface MBNNCacheHandle : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull id<MBXCancelable>)isRoadGraphDataUpdateAvailableForCallback:(nonnull MBNNRoadGraphUpdateAvailabilityCallback)callback __attribute((ns_returns_retained));
- (void)getCurrentRoadGraphVersionInfoForCallback:(nonnull MBNNRoadGraphVersionInfoCallback)callback
                                          timeout:(nullable NSNumber *)timeout __attribute__((deprecated));
- (nonnull id<MBXCancelable>)getCurrentRoadGraphVersionInfoForCallback:(nonnull MBNNRoadGraphVersionInfoCallback)callback __attribute((ns_returns_retained));

@end
