// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPosition;

NS_SWIFT_NAME(MatchedPointLocation)
__attribute__((visibility ("default")))
@interface MBNNMatchedPointLocation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * position on graph
 */
- (nonnull MBNNPosition *)getPosition __attribute((ns_returns_retained));

@end
