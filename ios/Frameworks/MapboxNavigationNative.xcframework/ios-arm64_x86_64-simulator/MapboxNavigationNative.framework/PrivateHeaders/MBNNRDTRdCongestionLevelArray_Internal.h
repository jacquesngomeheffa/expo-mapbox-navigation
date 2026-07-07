// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRDTRdCongestionLevel_Internal.h>

NS_SWIFT_NAME(RdCongestionLevelArray)
__attribute__((visibility ("default")))
@interface MBNNRDTRdCongestionLevelArray : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (MBNNRDTRdCongestionLevel)getForIndex:(uint64_t)index;
- (uint64_t)size;

@end
