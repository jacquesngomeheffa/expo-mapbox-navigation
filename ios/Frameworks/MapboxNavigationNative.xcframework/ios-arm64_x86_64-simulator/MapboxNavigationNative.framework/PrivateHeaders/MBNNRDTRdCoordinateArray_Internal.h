// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
@class MBXCoordinate2D;

NS_SWIFT_NAME(RdCoordinateArray)
@protocol MBNNRDTRdCoordinateArray
- (CLLocationCoordinate2D)getForIndex:(uint64_t)index;
- (uint64_t)size;
- (nonnull NSArray<MBXCoordinate2D *> *)rawArray;
@end
