// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteIdentifier;

NS_SWIFT_NAME(RouteIndices)
__attribute__((visibility ("default")))
@interface MBNNRouteIndices : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithRouteId:(nonnull MBNNRouteIdentifier *)routeId
                               legIndex:(uint32_t)legIndex
                              stepIndex:(uint32_t)stepIndex
                          geometryIndex:(uint32_t)geometryIndex
                          legShapeIndex:(uint32_t)legShapeIndex
                      intersectionIndex:(uint32_t)intersectionIndex
                      isForkPointPassed:(BOOL)isForkPointPassed;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Unique route id includes request UUID and route index
 */
@property (nonatomic, readonly, nonnull) MBNNRouteIdentifier *routeId;

/** index of current leg in route bounds */
@property (nonatomic, readonly) uint32_t legIndex;

/** index of current step in leg bounds */
@property (nonatomic, readonly) uint32_t stepIndex;

/** index in full route geometry */
@property (nonatomic, readonly) uint32_t geometryIndex;

/** index in leg shape */
@property (nonatomic, readonly) uint32_t legShapeIndex;

/** index in step bounds(i.e. on each new step we start indexing from 0) */
@property (nonatomic, readonly) uint32_t intersectionIndex;

/**
 *  True if the alternative is passed and probably will soon be removed.
 *  SDK should hide this route from UI, but keep it in the list of alternatives, as due to map matching error it may become a main route.
 */
@property (nonatomic, readonly) BOOL isForkPointPassed;


- (BOOL)isEqualToRouteIndices:(nonnull MBNNRouteIndices *)other;

@end
