// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdCongestionLevelArray;
@class MBNNRDTRdF64Array;
@class MBNNRDTRdSpeedLimitAnnotationArray;
@class MBNNRDTRdU32OptionalArray;

NS_SWIFT_NAME(RdAnnotation)
__attribute__((visibility ("default")))
@interface MBNNRDTRdAnnotation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nullable MBNNRDTRdCongestionLevelArray *)congestion __attribute((ns_returns_retained));
- (nullable MBNNRDTRdU32OptionalArray *)congestionNumeric __attribute((ns_returns_retained));
- (nullable MBNNRDTRdF64Array *)distance __attribute((ns_returns_retained));
- (nullable MBNNRDTRdF64Array *)duration __attribute((ns_returns_retained));
- (nullable MBNNRDTRdF64Array *)speed __attribute((ns_returns_retained));
- (nullable MBNNRDTRdSpeedLimitAnnotationArray *)maxspeed __attribute((ns_returns_retained));

@end
