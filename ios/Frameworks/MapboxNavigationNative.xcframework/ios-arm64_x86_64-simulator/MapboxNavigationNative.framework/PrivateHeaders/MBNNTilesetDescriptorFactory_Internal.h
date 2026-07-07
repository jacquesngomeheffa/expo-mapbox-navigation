// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNCacheHandle;
@class MBXTilesetDescriptor;

NS_SWIFT_NAME(TilesetDescriptorFactory)
__attribute__((visibility ("default")))
@interface MBNNTilesetDescriptorFactory : NSObject

/**
 * Creates TilesetDescriptor using the specified `dataset` and `version` resolved initially.
 * @param dataset       TilesetDescriptor dataset name
 * @param version       TilesetDescriptor version
 * @param includeAdas   true to include ADAS tiles
 * Note, the exact version must be provided.
 */
+ (nonnull MBXTilesetDescriptor *)buildForDataset:(nonnull NSString *)dataset
                                          version:(nonnull NSString *)version
                                      includeAdas:(BOOL)includeAdas __attribute((ns_returns_retained));
+ (nonnull MBXTilesetDescriptor *)getSpecificVersionForCache:(nonnull MBNNCacheHandle *)cache
                                                     version:(nonnull NSString *)version
                                                 includeAdas:(BOOL)includeAdas __attribute((ns_returns_retained));
+ (nonnull MBXTilesetDescriptor *)getLatestForCache:(nonnull MBNNCacheHandle *)cache
                                        includeAdas:(BOOL)includeAdas __attribute((ns_returns_retained));

@end
