// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNUserFeedbackMetadata;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * User feedback handle provides a way to get User Feedback metadata.
 */
NS_SWIFT_NAME(UserFeedbackHandle)
__attribute__((visibility ("default")))
@interface MBNNUserFeedbackHandle : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Gets user feedback metadata.
 * If some data accumulation (e.g., "locationsAfter") is not finished yet - it will be forcibly stopped.
 */
- (nonnull MBNNUserFeedbackMetadata *)getMetadata __attribute((ns_returns_retained));

@end
