// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNGraphPosition;

NS_SWIFT_NAME(MapMatch)
__attribute__((visibility ("default")))
@interface MBNNMapMatch : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;


/** probability of user is actually on this position */
@property (nonatomic, readonly) float proba;

/** original candidate index fetched around raw location before sorting by proba. For internal use */
@property (nonatomic, readonly) uint32_t fetchedCandidateIndex;


@end
