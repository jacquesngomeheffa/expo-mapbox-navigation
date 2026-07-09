// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXExpected<__covariant Value, __covariant Error>;

@class MBNNRefreshRouteResult;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Callback which is getting called to report refreshRoute result.
 * @param   route Updated route if the annotations could be updated, error string if not (wrong number of annotations)
 */
NS_SWIFT_NAME(RefreshRouteCallback)
typedef void (^MBNNRefreshRouteCallback)(MBXExpected<MBNNRefreshRouteResult *, NSString *> * _Nonnull result); // NOLINT(modernize-use-using)
