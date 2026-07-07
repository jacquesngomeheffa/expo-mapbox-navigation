// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteAlternative;
@protocol MBNNRouteInterface;

NS_SWIFT_NAME(RouteAlternativesObserver)
@protocol MBNNRouteAlternativesObserver
- (void)onRouteAlternativesChangedForRouteAlternatives:(nonnull NSArray<MBNNRouteAlternative *> *)routeAlternatives
                                               removed:(nonnull NSArray<MBNNRouteAlternative *> *)removed __attribute__((deprecated));
/**
 *  This callback is invoked when current primary route has `Onboard` origin, and some incoming route is a sub-route of the primary route and it has `Online` origin.
 *  @param onlinePrimaryRoute  new incoming route, that has `Online` origin and is a sub-route of primary route.
 */
- (void)onOnlinePrimaryRouteAvailableForOnlinePrimaryRoute:(nonnull id<MBNNRouteInterface>)onlinePrimaryRoute __attribute__((deprecated));
- (void)onRouteAlternativesUpdatedForOnlinePrimaryRoute:(nullable id<MBNNRouteInterface>)onlinePrimaryRoute
                                           alternatives:(nonnull NSArray<MBNNRouteAlternative *> *)alternatives
                                    removedAlternatives:(nonnull NSArray<MBNNRouteAlternative *> *)removedAlternatives;
/** This callback is invoked when an error occurs. */
- (void)onErrorForMessage:(nonnull NSString *)message;
@end
