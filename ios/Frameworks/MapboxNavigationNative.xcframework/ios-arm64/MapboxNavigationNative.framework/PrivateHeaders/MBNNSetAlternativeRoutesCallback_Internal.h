// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXExpected<__covariant Value, __covariant Error>;

@class MBNNRouteAlternative;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Callback which is getting called to report setAlternativeRoutes result.
 * @param   result  collection of alternatives on success, error string on failure.
 */
NS_SWIFT_NAME(SetAlternativeRoutesCallback)
typedef void (^MBNNSetAlternativeRoutesCallback)(MBXExpected<NSArray<MBNNRouteAlternative *> *, NSString *> * _Nonnull result); // NOLINT(modernize-use-using)
