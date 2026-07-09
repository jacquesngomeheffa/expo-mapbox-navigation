// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Defines the GNSS spoofing detection status.
 *
 * (e.g., Can be populated from u-blox UBX-SEC-SIG message).
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNSpoofingStatus)
{
    /** @brief Spoofing detection is not active or state is unknown. */
    MBNNSpoofingStatusUnknown = 0,
    /** @brief No spoofing detected. */
    MBNNSpoofingStatusNotDetected = 1,
    /** @brief Spoofing has been detected. */
    MBNNSpoofingStatusDetected = 2
} NS_SWIFT_NAME(SpoofingStatus);

NSString* MBNNSpoofingStatusToString(MBNNSpoofingStatus spoofing_status);
