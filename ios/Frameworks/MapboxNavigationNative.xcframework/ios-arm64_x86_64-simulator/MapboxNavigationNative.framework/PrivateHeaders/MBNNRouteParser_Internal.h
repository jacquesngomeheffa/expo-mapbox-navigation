// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRouteParserCallback_Internal.h>
@class MBXDataRef;
@class MBXExpected<__covariant Value, __covariant Error>;

@protocol MBNNRouteInterface;
@protocol MBNNRoutesData;
typedef NS_ENUM(NSInteger, MBNNRouterOrigin);

NS_SWIFT_NAME(RouteParser)
__attribute__((visibility ("default")))
@interface MBNNRouteParser : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Parse direction's response JSON to array of route objects.
 * NB: parsing can take significant time, so worth running it asynchronously, or use async version with callback
 * @param  response  Full routes response JSON from directions as a `string`.
 * @param  request   Corresponding URI request to directions.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, directions API or some custom?)
 * @return   Array of parsed route objects in the same order as in the "routes" array from response JSON.
 *           Or with error string in error case.
 */
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseDirectionsResponseForResponse:(nonnull NSString *)response
                                                                                                   request:(nonnull NSString *)request
                                                                                               routeOrigin:(MBNNRouterOrigin)routeOrigin __attribute((ns_returns_retained)) __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Parse direction's response JSON to array of route objects.
 * NB: parsing can take significant time, so worth running it asynchronously, or use async version with callback
 * @param  response  Full routes response JSON from directions as a `data_ref`.
 * @param  request   Corresponding URI request to directions.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, directions API or some custom?)
 * @return   Array of parsed route objects in the same order as in the "routes" array from response JSON.
 *           Or with error string in error case.
 */
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseDirectionsResponseForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                                                                                          request:(nonnull NSString *)request
                                                                                                      routeOrigin:(MBNNRouterOrigin)routeOrigin __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * EXPERIMENTAL API.
 * Same as `parseDirectionsResponse` but additionally parses `response` via directions-route/flatbuffers/directions_route_context.fbs and
 * fills `RouteInterface.getDirectionsRouteContextRef()` with the parsed data for each route.
 */
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseDirectionsResponseNROForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                                                                                             request:(nonnull NSString *)request
                                                                                                         routeOrigin:(MBNNRouterOrigin)routeOrigin __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Asynchronously parse direction's response JSON to array of route objects.
 * The callback will be invoked on the current run loop for the thread in which the function was invoked.
 * @param  response  Full routes response JSON from directions as a `string`.
 * @param  request   Corresponding URI request to directions.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, directions API or some custom?)
 * @param  callback  Callback with array of parsed route objects in the same order as in the "routes" array from response JSON.
 *                   Or error string in error case.
 */
+ (void)parseDirectionsResponseForResponse:(nonnull NSString *)response
                                   request:(nonnull NSString *)request
                               routeOrigin:(MBNNRouterOrigin)routeOrigin
                                  callback:(nonnull MBNNRouteParserCallback)callback __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Asynchronously parse direction's response JSON to array of route objects.
 * The callback will be invoked on the current run loop for the thread in which the function was invoked.
 * @param  response  Full routes response JSON from directions as a `data_ref`.
 * @param  request   Corresponding URI request to directions.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, directions API or some custom?)
 * @param  callback  Callback with array of parsed route objects in the same order as in the "routes" array from response JSON.
 *                   Or error string in error case.
 */
+ (void)parseDirectionsResponseForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                          request:(nonnull NSString *)request
                                      routeOrigin:(MBNNRouterOrigin)routeOrigin
                                         callback:(nonnull MBNNRouteParserCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * EXPERIMENTAL API.
 * Same as `parseDirectionsRoutes` but additionally parces `response` via directions-route/flatbuffers/directions_route_context.fbs and
 * fills `RouteInterface.getDirectionsRouteContextRef()` with the parsed data for each route.
 */
+ (void)parseDirectionsResponseNROForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                             request:(nonnull NSString *)request
                                         routeOrigin:(MBNNRouterOrigin)routeOrigin
                                            callback:(nonnull MBNNRouteParserCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Parse routes from a direction's response JSON to array of route objects.
 * This function should be used in case we don't have a full Directions routes response. Only routes.
 * This function restore a waypoints using information from routes and request.
 * To get JSON with restored waypoints use `RouteInterface.getWaypointsJson`
 * NB: parsing can take significant time, so worth running it asynchronously, or use async version with callback
 * @param  response  JSON object with `routes` array from directions (without any other data from full response).
 * @param  request   Corresponding URI request to directions.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, directions API or some custom?)
 * @return   Array of parsed route objects in the same order as in the "routes" array from response JSON.
 *           Or with error string in error case.
 */
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseDirectionsRoutesForResponse:(nonnull NSString *)response
                                                                                                 request:(nonnull NSString *)request
                                                                                             routeOrigin:(MBNNRouterOrigin)routeOrigin __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Asynchronously parse routes from a direction's response JSON to array of route objects.
 * This function should be used in case we don't have a full Directions routes response. Only routes.
 * This function restore a waypoints using information from routes and request.
 * To get JSON with restored waypoints use `RouteInterface.getWaypointsJson`
 * The callback will be invoked on the current run loop for the thread in which the function was invoked.
 * @param  response  JSON object with `routes` array from directions (without any other data from full response).
 * @param  request   Corresponding URI request to directions.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, directions API or some custom?)
 * @param  callback  Callback with array of parsed route objects in the same order as in the "routes" array from response JSON.
 *                   Or error string in error case.
 */
+ (void)parseDirectionsRoutesForResponse:(nonnull NSString *)response
                                 request:(nonnull NSString *)request
                             routeOrigin:(MBNNRouterOrigin)routeOrigin
                                callback:(nonnull MBNNRouteParserCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Asynchronously parse MapMatchingApi response JSON to array of route objects.
 * The callback will be invoked on the current run loop for the thread in which the function was invoked.
 * @param  response  Full response JSON from MapMatchingApi as a `string`.
 * @param  request   Corresponding URI request to MapMatchingApi.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, online or some custom?)
 * @param  callback  Callback with array of parsed route objects in the same order as in the "matchings" array from response JSON.
 *                   Or error string in error case.
 *                   Warning! Do not pass parsed RouteInterfaces as alternatives.
 */
+ (void)parseMapMatchingResponseForResponse:(nonnull NSString *)response
                                    request:(nonnull NSString *)request
                               routerOrigin:(MBNNRouterOrigin)routerOrigin
                                   callback:(nonnull MBNNRouteParserCallback)callback __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Asynchronously parse MapMatchingApi response JSON to array of route objects.
 * The callback will be invoked on the current run loop for the thread in which the function was invoked.
 * @param  response  Full response JSON from MapMatchingApi as a `data_ref`.
 * @param  request   Corresponding URI request to MapMatchingApi.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, online or some custom?)
 * @param  callback  Callback with array of parsed route objects in the same order as in the "matchings" array from response JSON.
 *                   Or error string in error case.
 *                   Warning! Do not pass parsed RouteInterfaces as alternatives.
 */
+ (void)parseMapMatchingResponseForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                           request:(nonnull NSString *)request
                                      routerOrigin:(MBNNRouterOrigin)routerOrigin
                                          callback:(nonnull MBNNRouteParserCallback)callback;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Parse MapMatchingApi response JSON to array of route objects.
 * NB: parsing can take significant time, so worth running it asynchronously, or use async version with callback
 * @param  response  Full routes response JSON from MapMatchingApi as a `string`.
 * @param  request   Corresponding URI request to MapMatchingApi.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, online or some custom?)
 * @return   Array of parsed route objects in the same order as in the "matchings" array from response JSON.
 *           Or with error string in error case.
 *           Warning! Do not pass parsed RouteInterfaces as alternatives.
 */
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseMapMatchingResponseForResponse:(nonnull NSString *)response
                                                                                                    request:(nonnull NSString *)request
                                                                                               routerOrigin:(MBNNRouterOrigin)routerOrigin __attribute((ns_returns_retained)) __attribute__((deprecated));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Parse MapMatchingApi response JSON to array of route objects.
 * NB: parsing can take significant time, so worth running it asynchronously, or use async version with callback
 * @param  response  Full routes response JSON from MapMatchingApi as a `data_ref`.
 * @param  request   Corresponding URI request to MapMatchingApi.
 * @param  routeOrigin  what kind of router we got this route from? (on-board, online or some custom?)
 * @return   Array of parsed route objects in the same order as in the "matchings" array from response JSON.
 *           Or with error string in error case.
 *           Warning! Do not pass parsed RouteInterfaces as alternatives.
 */
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseMapMatchingResponseForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                                                                                           request:(nonnull NSString *)request
                                                                                                      routerOrigin:(MBNNRouterOrigin)routerOrigin __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Creates an instance of `RoutesData` and calculate `RouteAlternative` metadata for each route in `alternativeRoutes` array.
 * The size of returned `alternativeRoutes` can be less then size of provided `alternativeRoutes` in case
 * if provided `alternativeRoutes` do not have a valid fork point with the `primaryRoute`.
 * For example if there is no one shared intersection, or there is no different intersections after the first shared intersection.
 */
+ (nonnull id<MBNNRoutesData>)createRoutesDataForPrimaryRoute:(nonnull id<MBNNRouteInterface>)primaryRoute
                                            alternativeRoutes:(nonnull NSArray<id<MBNNRouteInterface>> *)alternativeRoutes __attribute((ns_returns_retained));

@end
