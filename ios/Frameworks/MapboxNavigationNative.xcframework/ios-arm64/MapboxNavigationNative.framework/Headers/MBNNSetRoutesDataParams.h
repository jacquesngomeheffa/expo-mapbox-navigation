// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@protocol MBNNRoutesData;

NS_SWIFT_NAME(SetRoutesDataParams)
__attribute__((visibility ("default")))
@interface MBNNSetRoutesDataParams : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;


/** Leg index of primary route. Can be >0 in the case of switch from alternative route back to main route. */
@property (nonatomic, readonly) uint32_t legIndex;


@end
