// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNCurrentTilesVersionCallback_Internal.h>
#import <MapboxNavigationNative/MBNNOnVersionChangedCallback_Internal.h>
#import <MapboxNavigationNative/MBNNTilesUpdateAvailabilityCallback_Internal.h>

@class MBNNConfigHandle;
@class MBNNHistoryRecorderHandle;
@class MBNNTilesConfig;
@class MBXTilesetDescriptor;
@protocol MBXCancelable;
typedef NS_ENUM(NSInteger, MBNNBillingProductType);
typedef NS_ENUM(NSInteger, MBXTileDataDomain);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Tiles version manager. A controller that handles different versions of tiles
 */
NS_SWIFT_NAME(TilesManagerHandle)
__attribute__((visibility ("default")))
@interface MBNNTilesManagerHandle : NSObject

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Constructs TilesManager object with given dependencies
 *
 * @param tilesConfig           Configuration for tiles host, version, cache folder etc.
 * @param config                Config created with `ConfigFactory`
 * @param historyRecorder       History recorder created with `HistoryRecorderHandle.build` method
 * @param frameworkTypeForSKU   Core Framework or UX Framework, for right billing
 */
+ (nonnull MBNNTilesManagerHandle *)buildForTilesConfig:(nonnull MBNNTilesConfig *)tilesConfig
                                                 config:(nonnull MBNNConfigHandle *)config
                                        historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                                    frameworkTypeForSKU:(MBNNBillingProductType)frameworkTypeForSKU __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Check if the newest tiles version exists
 *
 * @param callback   will be invoked after the request
 */
- (nonnull id<MBXCancelable>)checkUpdateAvailableForCallback:(nonnull MBNNTilesUpdateAvailabilityCallback)callback __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Offline cache will be used by Hybrid router as a fallback in case of routing on current tiles failed
 *
 * @param tilesConfig           Configuration for tiles host, version, cache folder etc.
 * @param config                Config created with `ConfigFactory`
 * @param historyRecorder       History recorder created with `HistoryRecorderHandle.build` method
 * @param frameworkTypeForSKU   Core Framework or UX Framework, for right billing
 */
- (void)addOfflineCacheForTilesConfig:(nonnull MBNNTilesConfig *)tilesConfig
                               config:(nullable MBNNConfigHandle *)config
                      historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                  frameworkTypeForSKU:(MBNNBillingProductType)frameworkTypeForSKU;
/**
 * Pin desired version of routing tiles to work with. Used for testing, internal tooling, demo, etc.
 *
 * @param version   tiles version
 */
- (void)pinVersionForVersion:(nonnull NSString *)version;
/**
 * Pin desired version of HD tiles to work with. Used for testing, internal tooling, demo, etc.
 *
 * @param version   tiles version
 */
- (void)pinHDVersionForVersion:(nonnull NSString *)version;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Retrieves current version of tiles in use asynchronously
 *
 * @param callback  will be invoked after tiles version will be resolved
 */
- (nonnull id<MBXCancelable>)currentVersionForCallback:(nonnull MBNNCurrentTilesVersionCallback)callback __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Subscribe to notifications about changes to the current version of tiles
 *
 * @param callback   will be invoked every time when tiles version changed
 */
- (void)subscribeOnVersionChangedForCallback:(nonnull MBNNOnVersionChangedCallback)callback;
/**
 * Creates TilesetDescriptor using dataset & version in use and specified domains
 *
 * @param domains   like Navigation, Adas, Maps, etc
 */
- (nonnull MBXTilesetDescriptor *)tilesetDescriptorForDomains:(nonnull NSArray<NSNumber *> *)domains __attribute((ns_returns_retained));
/**
 * Creates TilesetDescriptor using the specified `dataset` and `version` resolved initially.
 *
 * @param dataset   TilesetDescriptor dataset name
 * @param version   TilesetDescriptor version
 * @param domains   like Navigation, Adas, Maps, etc
 *
 * Note, the exact version must be provided.
 */
+ (nonnull MBXTilesetDescriptor *)tilesetDescriptorForDataset:(nonnull NSString *)dataset
                                                      version:(nonnull NSString *)version
                                                      domains:(nonnull NSArray<NSNumber *> *)domains __attribute((ns_returns_retained));

@end
