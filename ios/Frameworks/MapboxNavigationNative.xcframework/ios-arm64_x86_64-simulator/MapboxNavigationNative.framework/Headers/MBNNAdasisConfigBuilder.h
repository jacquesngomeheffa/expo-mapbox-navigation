// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXExpected<__covariant Value, __covariant Error>;

@class MBNNAdasisConfig;

NS_SWIFT_NAME(AdasisConfigBuilder)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigBuilder : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/** Returns default ADASIS v2 config. */
+ (nonnull MBNNAdasisConfig *)defaultOptions __attribute((ns_returns_retained));

@end
