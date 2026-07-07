// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRDTRdSpeedUnits_Internal.h>

NS_SWIFT_NAME(RdSpeedLimitAnnotation)
__attribute__((visibility ("default")))
@interface MBNNRDTRdSpeedLimitAnnotation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithValue:(nullable NSNumber *)value;

- (nonnull instancetype)initWithUnits:(MBNNRDTRdSpeedUnits)units
                                value:(nullable NSNumber *)value;

@property (nonatomic, readonly) MBNNRDTRdSpeedUnits units;
@property (nonatomic, readonly, nullable) NSNumber *value;

@end
