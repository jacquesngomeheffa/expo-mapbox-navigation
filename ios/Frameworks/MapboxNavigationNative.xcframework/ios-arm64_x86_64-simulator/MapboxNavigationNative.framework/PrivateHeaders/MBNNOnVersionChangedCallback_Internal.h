// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRoadGraphVersionInfo;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Invoked on tiles version changed
 *
 * @param newVersionInfo  the version of tiles that was switched to
 */
NS_SWIFT_NAME(OnVersionChangedCallback)
typedef void (^MBNNOnVersionChangedCallback)(MBNNRoadGraphVersionInfo * _Nonnull newVersionInfo); // NOLINT(modernize-use-using)
