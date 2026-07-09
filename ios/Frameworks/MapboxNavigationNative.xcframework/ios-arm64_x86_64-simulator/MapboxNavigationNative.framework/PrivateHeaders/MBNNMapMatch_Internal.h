// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNMapMatch.h>

@class MBNNGraphPosition;

@interface MBNNMapMatch ()
- (nonnull instancetype)initWithPosition:(nonnull MBNNGraphPosition *)position
                                   proba:(float)proba
                   fetchedCandidateIndex:(uint32_t)fetchedCandidateIndex;
@property (nonatomic, readonly, nonnull) MBNNGraphPosition *position;
@end
