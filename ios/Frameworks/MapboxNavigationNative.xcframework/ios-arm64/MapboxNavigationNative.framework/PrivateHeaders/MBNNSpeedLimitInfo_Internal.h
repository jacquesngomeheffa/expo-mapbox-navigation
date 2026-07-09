// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNSpeedLimitRestriction;
typedef NS_ENUM(NSInteger, MBNNSpeedLimitType);
typedef NS_ENUM(NSInteger, MBNNSpeedLimitUnit);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Edge Speed limit info information
 */
NS_SWIFT_NAME(SpeedLimitInfo)
__attribute__((visibility ("default")))
@interface MBNNSpeedLimitInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithValue:(uint32_t)value
                                 unit:(MBNNSpeedLimitUnit)unit
                                 type:(MBNNSpeedLimitType)type
                          restriction:(nonnull MBNNSpeedLimitRestriction *)restriction;

/** Speed limit value in specified units */
@property (nonatomic, readonly) uint32_t value;

/** Speed limit unit */
@property (nonatomic, readonly) MBNNSpeedLimitUnit unit;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Type of Speed Limit from its source perspective: implicit or explicit
 */
@property (nonatomic, readonly) MBNNSpeedLimitType type;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Speed limit restriction info (see SpeedLimitRestriction)
 */
@property (nonatomic, readonly, nonnull) MBNNSpeedLimitRestriction *restriction;


- (BOOL)isEqualToSpeedLimitInfo:(nonnull MBNNSpeedLimitInfo *)other;

@end
