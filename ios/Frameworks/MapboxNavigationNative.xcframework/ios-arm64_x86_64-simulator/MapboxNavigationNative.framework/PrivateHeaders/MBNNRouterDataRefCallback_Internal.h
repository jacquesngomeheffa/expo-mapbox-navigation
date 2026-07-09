// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXDataRef;
@class MBXExpected<__covariant Value, __covariant Error>;

@class MBNNRouterError;
typedef NS_ENUM(NSInteger, MBNNRouterOrigin);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Returns either Directions/MapMatching API json as a `data_ref` or `RouterError`
 */
NS_SWIFT_NAME(RouterDataRefCallback)
typedef void (^MBNNRouterDataRefCallback)(MBXExpected<MBXDataRef *, NSArray<MBNNRouterError *> *> * _Nonnull routeResponse, MBNNRouterOrigin origin); // NOLINT(modernize-use-using)
