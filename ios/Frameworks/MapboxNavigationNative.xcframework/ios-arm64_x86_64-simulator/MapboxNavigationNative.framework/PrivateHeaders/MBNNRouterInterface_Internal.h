// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRouterDataRefCallback_Internal.h>
#import <MapboxNavigationNative/MBNNRouterRefreshCallback_Internal.h>

@class MBNNGetRouteOptions;
@class MBNNGetRouteSignature;
@class MBNNRouteRefreshOptions;

NS_SWIFT_NAME(RouterInterface)
@protocol MBNNRouterInterface
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Fetch routes based on Directions API URI.
 *
 * @param `directionsUri` Directions API request encoded to URI
 * @param `options` Directions API request options. For example request timeout. Can be empty object
 * @param `callback` Callback that gets notified with the results of the request
 *
 * @return cancellation token, can be used to cancel the request with `cancelRequest`
 */
- (uint64_t)getRouteForDirectionsUri:(nonnull NSString *)directionsUri
                             options:(nonnull MBNNGetRouteOptions *)options
                              caller:(nonnull MBNNGetRouteSignature *)caller
                     callbackDataRef:(nonnull MBNNRouterDataRefCallback)callbackDataRef;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Begins asynchronously refreshing the route with the given options and update specified route
 *
 * @param `options` Options described details about route refresh request
 * @param `callback` Callback that provides result of the route refresh. It includes json represented annotations or error, and info about router type
 *
 * @return cancellation token, can be used to cancel the request with `cancelRequest`
 */
- (uint64_t)getRouteRefreshForOptions:(nonnull MBNNRouteRefreshOptions *)options
                             callback:(nonnull MBNNRouterRefreshCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Fetch routes based on MapMatching API URI.
 *
 * @param `matchingUri` MapMatching API request encoded to URI
 * @param `options` MapMatching API request options. For example request timeout. Can be empty object
 * @param `callback` Callback that gets notified with the results of the request
 *
 * @return cancellation token, can be used to cancel the request with `cancelRequest`
 */
- (uint64_t)getRouteMapMatchedForMatchingUri:(nonnull NSString *)matchingUri
                                     options:(nonnull MBNNGetRouteOptions *)options
                             callbackDataRef:(nonnull MBNNRouterDataRefCallback)callbackDataRef;
/**
 * Cancels the `getRoute` request using token.
 *
 * @see `getRoute`. Cancels router request.
 */
- (void)cancelRouteRequestForToken:(uint64_t)token;
/**
 * Cancels the `getRouteRefresh` request using token.
 *
 * @see `getRouteRefresh`. Cancels router request.
 */
- (void)cancelRouteRefreshRequestForToken:(uint64_t)token;
/**
 * Cancels the `getRouteMapMatched` request using token.
 *
 * @see `getRouteMapMatched`. Cancels router request.
 */
- (void)cancelRouteMapMatchedRequestForToken:(uint64_t)token;
/** Interrupts all in-progress requests. */
- (void)cancelAll;
@end
