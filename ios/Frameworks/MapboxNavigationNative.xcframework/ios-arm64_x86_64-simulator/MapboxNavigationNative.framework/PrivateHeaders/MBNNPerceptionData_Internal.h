// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPerceptionFeature;

NS_SWIFT_NAME(PerceptionData)
__attribute__((visibility ("default")))
@interface MBNNPerceptionData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithMonotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds
                                                     features:(nonnull NSArray<MBNNPerceptionFeature *> *)features;

/**
 * Time when the data was created. The timestamp must be monotonic.
 * The timestamp should be captured as close as possible to the moment the data is produced
 * by the perception system, ideally at the sensor acquisition or feature extraction (fusion) step.
 */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Detailed representation of extracted road features.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNPerceptionFeature *> *features;


- (BOOL)isEqualToPerceptionData:(nonnull MBNNPerceptionData *)other;

@end
