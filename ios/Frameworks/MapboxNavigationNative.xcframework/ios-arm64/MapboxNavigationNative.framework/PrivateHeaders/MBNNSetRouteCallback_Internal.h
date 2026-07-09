// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXExpected<__covariant Value, __covariant Error>;

@class MBNNSetRoutesResult;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Callback which is getting called to report setRoute result.
 * @param   setRouteResult  filled `SetRoutesResult` on success, error string on failure. In case of unsetting route, returns SetRoutesResult with empty alerts.
 */
NS_SWIFT_NAME(SetRouteCallback)
typedef void (^MBNNSetRouteCallback)(MBXExpected<MBNNSetRoutesResult *, NSString *> * _Nonnull setRouteResult); // NOLINT(modernize-use-using)
