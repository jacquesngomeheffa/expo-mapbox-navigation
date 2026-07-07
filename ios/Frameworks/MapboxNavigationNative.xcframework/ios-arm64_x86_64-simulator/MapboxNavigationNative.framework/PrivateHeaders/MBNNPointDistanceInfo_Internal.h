// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPosition;

NS_SWIFT_NAME(PointDistanceInfo)
__attribute__((visibility ("default")))
@interface MBNNPointDistanceInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithDistance:(double)distance
                                position:(nonnull MBNNPosition *)position;

/** distance to point in meters */
@property (nonatomic, readonly) double distance;

@property (nonatomic, readonly, nonnull) MBNNPosition *position;

@end
