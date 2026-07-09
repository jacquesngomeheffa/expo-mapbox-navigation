// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRoadGraphVersionInfo;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *
 * @param isVersionResolved: returns true when the resolve routine has ended. false - when the version is still being searched.
 * @param currentVersionInfo: `RoadGraphVersionInfo` when the version is available.
 *                            `null` - when the version is still resolving or when the version could not be identified.
 */
NS_SWIFT_NAME(CurrentTilesVersionCallback)
typedef void (^MBNNCurrentTilesVersionCallback)(BOOL isVersionResolved, MBNNRoadGraphVersionInfo * _Nullable currentVersionInfo); // NOLINT(modernize-use-using)
