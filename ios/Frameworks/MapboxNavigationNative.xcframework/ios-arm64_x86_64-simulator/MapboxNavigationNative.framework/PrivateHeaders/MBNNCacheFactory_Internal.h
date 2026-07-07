// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNCacheFactory.h>
#import <MapboxNavigationNative/MBNNBillingProductType_Internal.h>

@interface MBNNCacheFactory ()
+ (nonnull MBNNCacheHandle *)buildForTilesConfig:(nonnull MBNNTilesConfig *)tilesConfig
                                          config:(nonnull MBNNConfigHandle *)config
                                 historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder
                             frameworkTypeForSKU:(MBNNBillingProductType)frameworkTypeForSKU __attribute((ns_returns_retained));
@end
