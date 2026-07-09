// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNCacheHandle;
@class MBXTilesetDescriptor;

NS_SWIFT_NAME(TilesetDescriptorFactory)
__attribute__((visibility ("default")))
__attribute__((deprecated))
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
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Gets TilesetDescriptor which corresponds to current Navigator dataset and the specified `version`.
 * @param cache         cache created with `CacheFactory`
 * @param version       TilesetDescriptor version
 * @param includeAdas   true to include ADAS tiles
 * Note, the exact version must be provided.
 */
+ (nonnull MBXTilesetDescriptor *)getSpecificVersionForCache:(nonnull MBNNCacheHandle *)cache
                                                     version:(nonnull NSString *)version
                                                 includeAdas:(BOOL)includeAdas __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Gets TilesetDescriptor which corresponds to the latest available version of routing tiles.
 * Intended for using when creating off-line tile packs.
 * @param cache         cache created with `CacheFactory`
 * Note, this method does not support ADAS tiles so far.
 */
+ (nonnull MBXTilesetDescriptor *)getLatestForCache:(nonnull MBNNCacheHandle *)cache
                                        includeAdas:(BOOL)includeAdas __attribute((ns_returns_retained));

@end
