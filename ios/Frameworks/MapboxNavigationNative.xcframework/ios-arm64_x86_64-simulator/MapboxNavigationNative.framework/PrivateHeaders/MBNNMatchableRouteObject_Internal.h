// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The record represents an object from the route response which could be matched to the HD graph.
 * The object may be a point (described with `distanceFromStartOfRoute`)
 * or a polyline (described with `distanceFromStartOfRoute` and `length`).
 */
NS_SWIFT_NAME(MatchableRouteObject)
__attribute__((visibility ("default")))
@interface MBNNMatchableRouteObject : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(nonnull NSString *)id_
          distanceFromStartOfRoute:(double)distanceFromStartOfRoute
                            length:(nullable NSNumber *)length
                           routeId:(nonnull NSString *)routeId;

/** ID of the resulting object. */
@property (nonatomic, readonly, nonnull, copy) NSString *id;

/** Distance from the start of the route. */
@property (nonatomic, readonly) double distanceFromStartOfRoute;

/**
 * Length of a polyline along the route.
 * Empty if the object is a point.
 */
@property (nonatomic, readonly, nullable) NSNumber *length;

/** ID of the route the object belongs to. */
@property (nonatomic, readonly, nonnull, copy) NSString *routeId;


@end
