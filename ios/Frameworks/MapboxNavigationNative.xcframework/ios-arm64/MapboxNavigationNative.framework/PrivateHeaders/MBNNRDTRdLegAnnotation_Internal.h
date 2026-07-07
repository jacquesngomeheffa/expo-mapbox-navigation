// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRDTRdAnnotation;
@class MBNNRDTRdLegClosureArray;

NS_SWIFT_NAME(RdLegAnnotation)
__attribute__((visibility ("default")))
@interface MBNNRDTRdLegAnnotation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull MBNNRDTRdAnnotation *)annotation __attribute((ns_returns_retained));
- (nonnull MBNNRDTRdLegClosureArray *)closures __attribute((ns_returns_retained));

@end
