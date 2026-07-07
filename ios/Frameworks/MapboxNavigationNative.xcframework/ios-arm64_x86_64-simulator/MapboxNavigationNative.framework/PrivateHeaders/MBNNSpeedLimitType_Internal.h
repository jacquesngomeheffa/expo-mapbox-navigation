// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNSpeedLimitType)
{
    /** Means no sign, limit is set by regulations for urban / rural / living_street */
    MBNNSpeedLimitTypeImplicit,
    /** Edge starts with a speed limit sign */
    MBNNSpeedLimitTypeExplicit,
    /** No exact information on presence of sign */
    MBNNSpeedLimitTypeUnknown,
    /** Edge does not start the way, no sign on the edge. Speed limit time is the same of on previous edge */
    MBNNSpeedLimitTypeProlonged
} NS_SWIFT_NAME(SpeedLimitType);

NSString* MBNNSpeedLimitTypeToString(MBNNSpeedLimitType speed_limit_type);
