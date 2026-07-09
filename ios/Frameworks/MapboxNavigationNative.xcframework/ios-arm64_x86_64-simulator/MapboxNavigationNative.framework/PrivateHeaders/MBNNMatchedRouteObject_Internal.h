// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXGeometry;

@class MBNNLaneGroupPosition;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The record represents an object from the route response matched to the HD graph.
 */
NS_SWIFT_NAME(MatchedRouteObject)
__attribute__((visibility ("default")))
@interface MBNNMatchedRouteObject : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(nonnull NSString *)id_
                             shape:(nonnull MBXGeometry *)shape
                firstPointPosition:(nonnull MBNNLaneGroupPosition *)firstPointPosition
                 lastPointPosition:(nullable MBNNLaneGroupPosition *)lastPointPosition;

/** ID of the resulting object. */
@property (nonatomic, readonly, nonnull, copy) NSString *id;

/**
 * Shape of the object.
 * Consists of one element if the object is a point.
 */
@property (nonatomic, readonly, nonnull) MBXGeometry *shape;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Position of the first point of the object's shape.
 */
@property (nonatomic, readonly, nonnull) MBNNLaneGroupPosition *firstPointPosition;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Position of the last point of the object's shape.
 * Empty if the object is a point.
 */
@property (nonatomic, readonly, nullable) MBNNLaneGroupPosition *lastPointPosition;


@end
