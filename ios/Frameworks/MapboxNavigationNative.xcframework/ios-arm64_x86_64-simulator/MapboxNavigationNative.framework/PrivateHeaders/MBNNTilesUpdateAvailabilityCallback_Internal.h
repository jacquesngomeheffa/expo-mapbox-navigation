// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRoadGraphVersionInfo;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Invoked after the check of tiles update availability
 *
 * @param isUpdateAvailable   true if update is available
 * @param newVersionInfo      new version info (dataset, version)
 */
NS_SWIFT_NAME(TilesUpdateAvailabilityCallback)
typedef void (^MBNNTilesUpdateAvailabilityCallback)(BOOL isUpdateAvailable, MBNNRoadGraphVersionInfo * _Nullable newVersionInfo); // NOLINT(modernize-use-using)
