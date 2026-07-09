// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNCacheFactory.h>

@interface MBNNCacheFactory ()
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Builds which can be re-used in various instances
 * @param tilesConfig     Configuration for tiles host, version, cache folder etc.
 * @param config          Config created with `ConfigFactory`
 * @param historyRecorder History recorder created with `HistoryRecorderHandle.build` method
 * @param frameworkTypeForSKU          Core Framework or UX Framework, for right billing
 */
+ (nonnull MBNNCacheHandle *)buildForTilesConfig:(nonnull MBNNTilesConfig *)tilesConfig
                                          config:(nonnull MBNNConfigHandle *)config
                                 historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                             frameworkTypeForSKU:(MBNNBillingProductType)frameworkTypeForSKU __attribute((ns_returns_retained));
@end
