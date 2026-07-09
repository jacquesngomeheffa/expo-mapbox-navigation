// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRoadGraphUpdateAvailabilityCallback_Internal.h>
#import <MapboxNavigationNative/MBNNRoadGraphVersionInfoCallback_Internal.h>

@protocol MBXCancelable;

NS_SWIFT_NAME(CacheHandle)
__attribute__((visibility ("default")))
__attribute__((deprecated))
@interface MBNNCacheHandle : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Async check if a road graph data update is available.
 * We determine the road graph version on the cache creation.
 * The next time a cache is created, it will also discover this version
 * and pick it up if automatic version switching is used.
 *
 * The check is guaranteed to take less than 5 seconds (default) and returns false in case of timeout.
 */
- (nonnull id<MBXCancelable>)isRoadGraphDataUpdateAvailableForCallback:(nonnull MBNNRoadGraphUpdateAvailabilityCallback)callback __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Retrieves current road graph version asynchronously.
 * The road graph version is determined on the cache creation
 * and stays the same throughout the whole navigator lifecycle.
 *
 * The retrieval process waits for the tiles config to resolve which may take some indefinite amount of time.
 * Once the version is resolved, the consequent calls should be almost instant.
 */
- (nonnull id<MBXCancelable>)getCurrentRoadGraphVersionInfoForCallback:(nonnull MBNNRoadGraphVersionInfoCallback)callback __attribute((ns_returns_retained));

@end
