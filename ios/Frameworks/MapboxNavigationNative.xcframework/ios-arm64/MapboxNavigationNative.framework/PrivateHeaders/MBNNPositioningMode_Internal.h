// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Defines the positioning mode and quality of the navigation solution.
 * This reflects the state of the sensor fusion system.
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNPositioningMode)
{
    /** @brief Positioning mode is not available. */
    MBNNPositioningModeUnknown = 0,
    /** @brief Position is not valid. */
    MBNNPositioningModeNoFix = 1,
    /**
     * @brief Position is propagated using sensors only (IMU, wheel speed, etc.).
     * This mode is active when no GNSS signals are being used.
     */
    MBNNPositioningModeDeadReckoningOnly = 2,
    /**
     * @brief GNSS-Only, Standard Precision (SPS).
     * An autonomous, non-augmented GNSS position.
     */
    MBNNPositioningModeGnssStandard = 10,
    /**
     * @brief GNSS-Only, Differential.
     * GNSS position augmented by differential corrections (SBAS, DGPS).
     */
    MBNNPositioningModeGnssDifferential = 11,
    /**
     * @brief GNSS-Only, High Precision (Float).
     * GNSS position using a high-precision floating ambiguity solution (e.g., RTK Float).
     */
    MBNNPositioningModeGnssHighPrecisionFloat = 12,
    /**
     * @brief GNSS-Only, High Precision (Fixed).
     * GNSS position using a high-precision fixed ambiguity solution (e.g., RTK Fixed, PPP).
     */
    MBNNPositioningModeGnssHighPrecisionFixed = 13,
    /** @brief Fused solution using Standard Precision GNSS. */
    MBNNPositioningModeFusedStandard = 20,
    /** @brief Fused solution using Differential GNSS. */
    MBNNPositioningModeFusedDifferential = 21,
    /** @brief Fused solution using High Precision Float GNSS. */
    MBNNPositioningModeFusedHighPrecisionFloat = 22,
    /** @brief Fused solution using High Precision Fixed GNSS. */
    MBNNPositioningModeFusedHighPrecisionFixed = 23
} NS_SWIFT_NAME(PositioningMode);

NSString* MBNNPositioningModeToString(MBNNPositioningMode positioning_mode);
