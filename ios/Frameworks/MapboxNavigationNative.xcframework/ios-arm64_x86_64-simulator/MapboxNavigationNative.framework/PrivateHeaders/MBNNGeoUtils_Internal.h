// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXGeometry;

NS_SWIFT_NAME(GeoUtils)
__attribute__((visibility ("default")))
@interface MBNNGeoUtils : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 *  Calculates the length of the given shape in meters.
 * @param shape Shape to calculate the length for
 * @return Length of the shape in meters
 */
+ (double)calculateShapeLengthForShape:(nonnull MBXGeometry *)shape;
/**
 *  Calculates the length of the given shape in meters.
 * @param shape Shape to calculate the length of a part for
 * @param startIndex Start index of the part to calculate the length for
 * @param endIndex End index of the part to calculate the length for
 * @return Length of the shape in meters
 */
+ (double)calculateShapeLengthForShape:(nonnull MBXGeometry *)shape
                            startIndex:(uint32_t)startIndex
                              endIndex:(uint32_t)endIndex;
/**
 * Provides the unique identifier for the given link. Link is a part of a road between two intersection.
 * The identifier is unique within the current road graph within a reasonable some area. It may be used to identify the link in the road graph.
 * The identifier is stable and does not change between different versions of the road network.
 * The identifier is not guaranteed to be unique between different versions of a map, if link geometry changes.
 * It is not guaranteed to be unique between different regions of the same map.
 * @param shape Shape to calculate the link id for
 * @param startIndex Start index of the link
 * @param endIndex End index of the link
 * @return Unique identifier of the link
 * Note, all the geometry points must not be equal.
 */
+ (uint64_t)getTopoLinkIdForShape:(nonnull MBXGeometry *)shape
                       startIndex:(uint32_t)startIndex
                         endIndex:(uint32_t)endIndex;

@end
