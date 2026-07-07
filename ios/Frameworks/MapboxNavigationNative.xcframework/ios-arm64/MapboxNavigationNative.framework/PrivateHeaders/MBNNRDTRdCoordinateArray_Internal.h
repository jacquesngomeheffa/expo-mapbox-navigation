// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
@class MBXCoordinate2D;

NS_SWIFT_NAME(RdCoordinateArray)
__attribute__((visibility ("default")))
@interface MBNNRDTRdCoordinateArray : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (CLLocationCoordinate2D)getForIndex:(uint64_t)index;
- (uint64_t)size;
- (nonnull NSArray<MBXCoordinate2D *> *)rawArray __attribute((ns_returns_retained));

@end
