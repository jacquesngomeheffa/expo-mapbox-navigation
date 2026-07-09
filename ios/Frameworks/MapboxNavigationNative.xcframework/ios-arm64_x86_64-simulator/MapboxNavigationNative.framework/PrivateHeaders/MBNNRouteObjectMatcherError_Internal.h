// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The error occurring when matching a route object failed.
 */
NS_SWIFT_NAME(RouteObjectMatcherError)
__attribute__((visibility ("default")))
@interface MBNNRouteObjectMatcherError : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithDescription:(nonnull NSString *)description
                              routeObjectId:(nonnull NSString *)routeObjectId;

/** Description of the error. */
@property (nonatomic, readonly, nonnull, copy) NSString *description;

/** ID of the route object failed to match. */
@property (nonatomic, readonly, nonnull, copy) NSString *routeObjectId;


@end
