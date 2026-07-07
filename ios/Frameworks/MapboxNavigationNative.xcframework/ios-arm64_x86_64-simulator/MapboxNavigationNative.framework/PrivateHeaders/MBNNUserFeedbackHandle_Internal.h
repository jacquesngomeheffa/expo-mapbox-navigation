// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNUserFeedbackMetadata;

NS_SWIFT_NAME(UserFeedbackHandle)
__attribute__((visibility ("default")))
@interface MBNNUserFeedbackHandle : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull MBNNUserFeedbackMetadata *)getMetadata __attribute((ns_returns_retained));

@end
