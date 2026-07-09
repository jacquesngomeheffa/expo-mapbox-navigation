// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Defines the RF jamming status.
 *
 * (e.g., Can be populated from u-blox UBX-SEC-SIG message).
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNJammingStatus)
{
    /** @brief Jamming state is unknown. */
    MBNNJammingStatusUnknown = 0,
    /** @brief No jamming detected. */
    MBNNJammingStatusOk = 1,
    /** @brief Potential jamming detected, solution may be degraded. */
    MBNNJammingStatusWarning = 2,
    /** @brief Critical jamming detected, solution is likely unusable. */
    MBNNJammingStatusCritical = 3
} NS_SWIFT_NAME(JammingStatus);

NSString* MBNNJammingStatusToString(MBNNJammingStatus jamming_status);
