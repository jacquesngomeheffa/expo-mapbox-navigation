// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdSpeedLimitAnnotation;

NS_SWIFT_NAME(RdSpeedLimitAnnotationArray)
__attribute__((visibility ("default")))
@interface MBNNRDTRdSpeedLimitAnnotationArray : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nullable MBNNRDTRdSpeedLimitAnnotation *)getForIndex:(uint64_t)index __attribute((ns_returns_retained));
- (uint64_t)size;

@end
