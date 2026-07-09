// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXDataRef;
@class MBXExpected<__covariant Value, __covariant Error>;

@class MBNNRouterError;
typedef NS_ENUM(NSInteger, MBNNRouterOrigin);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Refresh routes callback which receives Directions API response
 *
 * @param `annotationsResponse` Directions API response with updated route in json format
 * @param `origin` Refresh origin
 * @param `responseHeaders` Response HTTP headers with additional data (e.g. expiration time)
 */
NS_SWIFT_NAME(RouterRefreshCallback)
typedef void (^MBNNRouterRefreshCallback)(MBXExpected<MBXDataRef *, NSArray<MBNNRouterError *> *> * _Nonnull annotationsResponse, MBNNRouterOrigin origin, NSDictionary<NSString *, NSString *> * _Nonnull responseHeaders); // NOLINT(modernize-use-using)
