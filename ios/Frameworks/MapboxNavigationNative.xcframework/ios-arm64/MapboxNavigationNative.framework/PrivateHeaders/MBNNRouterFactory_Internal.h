// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNCacheHandle;
@class MBNNConfigHandle;
@class MBNNHistoryRecorderHandle;
@class MBNNTilesManagerHandle;
@protocol MBNNRouterInterface;
typedef NS_ENUM(NSInteger, MBNNRouterType);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Constructs a new router object with given dependencies
 * Note: To utilize navigator's hybrid router use Navigator.getRoute() method
 */
NS_SWIFT_NAME(RouterFactory)
__attribute__((visibility ("default")))
@interface MBNNRouterFactory : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Constructs a new router object
 *
 * @param type             indicate which kind of router should be created
 * @param cache            cache created with `CacheFactory`
 * @param config           config created with `ConfigFactory`
 * @param historyRecorder  history recorder created with `HistoryRecorderHandle::build()` method
 */
+ (nonnull id<MBNNRouterInterface>)buildForType:(MBNNRouterType)type
                                          cache:(nonnull MBNNCacheHandle *)cache
                                         config:(nonnull MBNNConfigHandle *)config
                                historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder __attribute((ns_returns_retained)) __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Constructs a new router object
 *
 * @param type             indicate which kind of router should be created
 * @param tilesManager     tiles manager created with 'TilesManagerHandle.build' method
 * @param config           config created with `ConfigFactory`
 * @param historyRecorder  history recorder created with `HistoryRecorderHandle::build()` method
 */
+ (nonnull id<MBNNRouterInterface>)buildForType:(MBNNRouterType)type
                                   tilesManager:(nonnull MBNNTilesManagerHandle *)tilesManager
                                         config:(nonnull MBNNConfigHandle *)config
                                historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder __attribute((ns_returns_retained));

@end
