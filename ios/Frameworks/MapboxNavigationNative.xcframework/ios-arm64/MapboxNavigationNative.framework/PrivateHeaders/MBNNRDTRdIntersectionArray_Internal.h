// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRDTRdIntersectionArrayForEachCallback.h>

@protocol MBNNRDTRdIntersection;

NS_SWIFT_NAME(RdIntersectionArray)
@protocol MBNNRDTRdIntersectionArray
- (nonnull id<MBNNRDTRdIntersection>)getForIndex:(uint64_t)index;
- (uint64_t)size;
- (void)forEachForCbk:(nonnull MBNNRDTRdIntersectionArrayForEachCallback)cbk;
@end
