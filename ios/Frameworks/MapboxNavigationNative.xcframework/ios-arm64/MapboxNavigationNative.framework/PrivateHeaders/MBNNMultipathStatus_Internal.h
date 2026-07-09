// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Defines the aggregated multipath detection status for the solution.
 *
 * This is an aggregated status for the whole fix, not per-satellite.
 * (e.g., Android's GnssMeasurement.getMultipathIndicator() would need to be
 * aggregated by the provider to populate this).
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNMultipathStatus)
{
    /** @brief The multipath state is unknown. */
    MBNNMultipathStatusUnknown = 0,
    /** @brief Multipath is not detected in the solution. */
    MBNNMultipathStatusNotDetected = 1,
    /** @brief Multipath is detected and may be affecting the solution. */
    MBNNMultipathStatusDetected = 2
} NS_SWIFT_NAME(MultipathStatus);

NSString* MBNNMultipathStatusToString(MBNNMultipathStatus multipath_status);
