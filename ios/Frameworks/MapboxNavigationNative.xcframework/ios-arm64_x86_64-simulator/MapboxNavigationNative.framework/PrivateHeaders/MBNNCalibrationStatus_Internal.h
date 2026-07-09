// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Defines the calibration or operational state of a sensor or system.
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNCalibrationStatus)
{
    /** @brief The sensor/system is off, not present, or not in use. */
    MBNNCalibrationStatusNotAvailable = 0,
    /**
     * @brief The system is on and actively performing initial calibration,
     * but its information is not yet usable.
     */
    MBNNCalibrationStatusInProgress = 1,
    /**
     * @brief A low-confidence, "coarse" calibration is available.
     * The information is usable but should be treated with high uncertainty.
     */
    MBNNCalibrationStatusCoarse = 2,
    /**
     * @brief The system is fully calibrated (fine-tuned) and is
     * providing high-confidence, stable information.
     */
    MBNNCalibrationStatusReady = 3,
    /**
     * @brief The calibration has failed or the system reported an
     * unrecoverable error and its data is not usable.
     */
    MBNNCalibrationStatusError = 4
} NS_SWIFT_NAME(CalibrationStatus);

NSString* MBNNCalibrationStatusToString(MBNNCalibrationStatus calibration_status);
