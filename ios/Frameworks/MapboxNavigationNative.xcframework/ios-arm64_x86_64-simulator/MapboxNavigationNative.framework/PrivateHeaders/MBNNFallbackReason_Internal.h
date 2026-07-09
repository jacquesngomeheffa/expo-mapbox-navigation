// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Internal nav-native enum used to determine the reason of fallback
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNFallbackReason)
{
    MBNNFallbackReasonNone,
    MBNNFallbackReasonCacheIsNotReady,
    MBNNFallbackReasonNonDrivingActiveGuidanceMode,
    MBNNFallbackReasonCustomExternalRouterOrigin,
    MBNNFallbackReasonAlwaysFallbackPolicy,
    MBNNFallbackReasonViterbiRouteMatcherError
} NS_SWIFT_NAME(FallbackReason);

NSString* MBNNFallbackReasonToString(MBNNFallbackReason fallback_reason);
