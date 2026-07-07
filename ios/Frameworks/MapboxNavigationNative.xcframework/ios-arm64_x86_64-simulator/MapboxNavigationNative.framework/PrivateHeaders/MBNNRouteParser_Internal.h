// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRouteParserCallback_Internal.h>
#import <MapboxNavigationNative/MBNNRouterOrigin.h>
@class MBXDataRef;
@class MBXExpected<__covariant Value, __covariant Error>;

@protocol MBNNRouteInterface;
@protocol MBNNRoutesData;

NS_SWIFT_NAME(RouteParser)
__attribute__((visibility ("default")))
@interface MBNNRouteParser : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseDirectionsResponseForResponse:(nonnull NSString *)response
                                                                                                   request:(nonnull NSString *)request
                                                                                               routeOrigin:(MBNNRouterOrigin)routeOrigin __attribute((ns_returns_retained)) __attribute__((deprecated));
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseDirectionsResponseForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                                                                                          request:(nonnull NSString *)request
                                                                                                      routeOrigin:(MBNNRouterOrigin)routeOrigin __attribute((ns_returns_retained));
+ (void)parseDirectionsResponseForResponse:(nonnull NSString *)response
                                   request:(nonnull NSString *)request
                               routeOrigin:(MBNNRouterOrigin)routeOrigin
                                  callback:(nonnull MBNNRouteParserCallback)callback __attribute__((deprecated));
+ (void)parseDirectionsResponseForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                          request:(nonnull NSString *)request
                                      routeOrigin:(MBNNRouterOrigin)routeOrigin
                                         callback:(nonnull MBNNRouteParserCallback)callback;
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseDirectionsRoutesForResponse:(nonnull NSString *)response
                                                                                                 request:(nonnull NSString *)request
                                                                                             routeOrigin:(MBNNRouterOrigin)routeOrigin __attribute((ns_returns_retained));
+ (void)parseDirectionsRoutesForResponse:(nonnull NSString *)response
                                 request:(nonnull NSString *)request
                             routeOrigin:(MBNNRouterOrigin)routeOrigin
                                callback:(nonnull MBNNRouteParserCallback)callback;
+ (void)parseMapMatchingResponseForResponse:(nonnull NSString *)response
                                    request:(nonnull NSString *)request
                               routerOrigin:(MBNNRouterOrigin)routerOrigin
                                   callback:(nonnull MBNNRouteParserCallback)callback __attribute__((deprecated));
+ (void)parseMapMatchingResponseForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                           request:(nonnull NSString *)request
                                      routerOrigin:(MBNNRouterOrigin)routerOrigin
                                          callback:(nonnull MBNNRouteParserCallback)callback;
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseMapMatchingResponseForResponse:(nonnull NSString *)response
                                                                                                    request:(nonnull NSString *)request
                                                                                               routerOrigin:(MBNNRouterOrigin)routerOrigin __attribute((ns_returns_retained)) __attribute__((deprecated));
+ (nonnull MBXExpected<NSArray<id<MBNNRouteInterface>> *, NSString *> *)parseMapMatchingResponseForResponseDataRef:(nonnull MBXDataRef *)responseDataRef
                                                                                                           request:(nonnull NSString *)request
                                                                                                      routerOrigin:(MBNNRouterOrigin)routerOrigin __attribute((ns_returns_retained));
+ (nonnull id<MBNNRoutesData>)createRoutesDataForPrimaryRoute:(nonnull id<MBNNRouteInterface>)primaryRoute
                                            alternativeRoutes:(nonnull NSArray<id<MBNNRouteInterface>> *)alternativeRoutes __attribute((ns_returns_retained));

@end
