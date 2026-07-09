// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNTilesEndpointConfig;
@class MBXTileStore;

NS_SWIFT_NAME(NavigationTilesConfig)
__attribute__((visibility ("default")))
@interface MBNNNavigationTilesConfig : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithTileStore:(nonnull MBXTileStore *)tileStore
                           endpointConfig:(nonnull MBNNTilesEndpointConfig *)endpointConfig
                         hdEndpointConfig:(nonnull MBNNTilesEndpointConfig *)hdEndpointConfig;

/** TileStore instance providing routing tiles */
@property (nonatomic, readonly, nonnull) MBXTileStore *tileStore;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Routing tiles configuration
 */
@property (nonatomic, readonly, nonnull) MBNNTilesEndpointConfig *endpointConfig;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * HD tiles configuration
 */
@property (nonatomic, readonly, nonnull) MBNNTilesEndpointConfig *hdEndpointConfig;


@end
