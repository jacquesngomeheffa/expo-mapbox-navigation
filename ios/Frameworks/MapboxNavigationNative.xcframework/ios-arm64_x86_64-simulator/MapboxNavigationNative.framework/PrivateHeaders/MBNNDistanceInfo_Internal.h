// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

NS_SWIFT_NAME(DistanceInfo)
__attribute__((visibility ("default")))
@interface MBNNDistanceInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithDistance:(double)distance
                                  length:(double)length;

@property (nonatomic, readonly) double distance;
@property (nonatomic, readonly) double length;

@end
