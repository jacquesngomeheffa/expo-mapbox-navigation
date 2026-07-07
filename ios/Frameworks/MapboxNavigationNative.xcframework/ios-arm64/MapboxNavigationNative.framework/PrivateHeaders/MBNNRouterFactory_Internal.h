// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRouterType_Internal.h>

@class MBNNCacheHandle;
@class MBNNConfigHandle;
@class MBNNHistoryRecorderHandle;
@protocol MBNNRouterInterface;

NS_SWIFT_NAME(RouterFactory)
__attribute__((visibility ("default")))
@interface MBNNRouterFactory : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

+ (nonnull id<MBNNRouterInterface>)buildForType:(MBNNRouterType)type
                                          cache:(nonnull MBNNCacheHandle *)cache
                                         config:(nonnull MBNNConfigHandle *)config
                                historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder __attribute((ns_returns_retained));

@end
