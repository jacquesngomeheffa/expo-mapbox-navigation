// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNRDTRdSpeedUnits);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Denotes speed limit at the given segment
 */
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
