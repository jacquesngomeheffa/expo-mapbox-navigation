// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNDetectedObject;

NS_SWIFT_NAME(DetectedObjects)
__attribute__((visibility ("default")))
@interface MBNNDetectedObjects : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithMonotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds
                                              detectedObjects:(nonnull NSArray<MBNNDetectedObject *> *)detectedObjects;

/** Time when the data was created. The timestamp must be a monotonic timestamp. */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * List of detected objects.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNDetectedObject *> *detectedObjects;


- (BOOL)isEqualToDetectedObjects:(nonnull MBNNDetectedObjects *)other;

@end
