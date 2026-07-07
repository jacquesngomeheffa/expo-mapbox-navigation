// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNAttitudeData;
@class MBNNPoint3d;

NS_SWIFT_NAME(MotionData)
__attribute__((visibility ("default")))
@interface MBNNMotionData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithAttitude:(nonnull MBNNAttitudeData *)attitude
                            rotationRate:(nonnull MBNNPoint3d *)rotationRate
                     gravityAcceleration:(nonnull MBNNPoint3d *)gravityAcceleration
                        userAcceleration:(nonnull MBNNPoint3d *)userAcceleration
                           magneticField:(nonnull MBNNPoint3d *)magneticField
                                 heading:(float)heading
           monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds;

@property (nonatomic, readonly, nonnull) MBNNAttitudeData *attitude __attribute__((deprecated));
@property (nonatomic, readonly, nonnull) MBNNPoint3d *rotationRate __attribute__((deprecated));
@property (nonatomic, readonly, nonnull) MBNNPoint3d *gravityAcceleration __attribute__((deprecated));
@property (nonatomic, readonly, nonnull) MBNNPoint3d *userAcceleration __attribute__((deprecated));
@property (nonatomic, readonly, nonnull) MBNNPoint3d *magneticField __attribute__((deprecated));
/** The heading angle (measured in degrees) relative to the current reference frame. */
@property (nonatomic, readonly) float heading __attribute__((deprecated));

/** monotonic timestamp in nanoseconds */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToMotionData:(nonnull MBNNMotionData *)other;

@end
