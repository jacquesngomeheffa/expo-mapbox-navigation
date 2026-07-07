// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdLegClosure;

NS_SWIFT_NAME(RdLegClosureArray)
__attribute__((visibility ("default")))
@interface MBNNRDTRdLegClosureArray : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull MBNNRDTRdLegClosure *)getForIndex:(uint64_t)index __attribute((ns_returns_retained));
- (uint64_t)size;

@end
