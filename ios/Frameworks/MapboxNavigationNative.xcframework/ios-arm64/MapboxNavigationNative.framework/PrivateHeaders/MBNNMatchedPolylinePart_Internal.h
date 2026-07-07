// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

NS_SWIFT_NAME(MatchedPolylinePart)
__attribute__((visibility ("default")))
@interface MBNNMatchedPolylinePart : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithBeginIndex:(uint32_t)beginIndex
                                  endIndex:(uint32_t)endIndex
                         amountOfKeypoints:(uint32_t)amountOfKeypoints;

/**
 * Begin index of a keypoint in the matched interval (included).
 * Currently might be only zero, partial matching works from start to end.
 */
@property (nonatomic, readonly) uint32_t beginIndex;

/** End index of a keypoint in the matched interval (not included) */
@property (nonatomic, readonly) uint32_t endIndex;

/** Total amount of keypoints */
@property (nonatomic, readonly) uint32_t amountOfKeypoints;


@end
