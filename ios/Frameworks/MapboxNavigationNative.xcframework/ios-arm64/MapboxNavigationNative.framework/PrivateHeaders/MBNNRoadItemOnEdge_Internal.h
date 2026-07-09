// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRoadItem;

NS_SWIFT_NAME(RoadItemOnEdge)
__attribute__((visibility ("default")))
@interface MBNNRoadItemOnEdge : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithShapeIndex:(float)shapeIndex
                              percentAlong:(double)percentAlong
                                  roadItem:(nonnull MBNNRoadItem *)roadItem;

/**
 * Position on edge is represented as a shape index.
 * Integer part is an index of edge segment and fraction
 * is a position on the segment: 0 - left point, 1 - right point,
 * 0.5 - in the middle between the segment points.
 * Ex.: 3.5 means the middle the the 3rd segment on the Edge shape, shape has more then 4 points
 */
@property (nonatomic, readonly) float shapeIndex;

/**
 * Position along edge shape [0-1].
 * The value's added for compliance with Navigator interface, functionally it duplicates `shapeIndex`
 */
@property (nonatomic, readonly) double percentAlong;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The road item.
 */
@property (nonatomic, readonly, nonnull) MBNNRoadItem *roadItem;


- (BOOL)isEqualToRoadItemOnEdge:(nonnull MBNNRoadItemOnEdge *)other;

@end
