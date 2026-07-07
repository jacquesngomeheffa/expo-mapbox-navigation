// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdStep;

NS_SWIFT_NAME(RdStepArray)
__attribute__((visibility ("default")))
@interface MBNNRDTRdStepArray : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull MBNNRDTRdStep *)getForIndex:(uint64_t)index __attribute((ns_returns_retained));
- (uint64_t)size;

@end
