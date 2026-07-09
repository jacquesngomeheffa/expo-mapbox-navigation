// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Defines the integrity level of the position fix.
 * Useful for understanding the trustworthiness of the location, especially for
 * ASIL-rated systems.
 *
 * Example: For u-blox receivers, this can be derived by comparing the
 * Horizontal Protection Level (HPL) from the UBX-NAV-PL message against a
 * defined Alert Limit (AL). If HPL < AL, the status is Safe.
 *
 * For standard Android, this is not directly provided. Unless a custom RAIM
 * implementation exists, 'Unknown' should be used.
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNIntegrityStatus)
{
    /** @brief The integrity status is unknown or not provided. */
    MBNNIntegrityStatusUnknown = 0,
    /** @brief The position is considered reliable (e.g., RAIM check passed). */
    MBNNIntegrityStatusSafe = 1,
    /** @brief The position has a reduced level of confidence. */
    MBNNIntegrityStatusWarning = 2,
    /** @brief The position should not be used for navigation. */
    MBNNIntegrityStatusUnsafe = 3
} NS_SWIFT_NAME(IntegrityStatus);

NSString* MBNNIntegrityStatusToString(MBNNIntegrityStatus integrity_status);
