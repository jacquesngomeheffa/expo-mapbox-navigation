// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@protocol MBNNRDTRdCongestionLevelArray;
@protocol MBNNRDTRdF64Array;
@protocol MBNNRDTRdSpeedLimitAnnotationArray;
@protocol MBNNRDTRdU32OptionalArray;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * An annotation object contains additional details about each line segment along the route geometry.
 *
 * \sa https://docs.mapbox.com/api/navigation/#route-leg-object
 */
NS_SWIFT_NAME(RdAnnotation)
@protocol MBNNRDTRdAnnotation
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The level of congestion between each entry in the array of coordinate pairs in the route leg.
 * @sa https://docs.mapbox.com/api/navigation/directions/#route-leg-object
 */
- (nullable id<MBNNRDTRdCongestionLevelArray>)congestion;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The numeric level (0-100) of congestion between each entry in the array of coordinate pairs in the route leg.
 * Null optional if congestion is unknown.
 * @sa https://docs.mapbox.com/api/navigation/directions/#route-leg-object
 */
- (nullable id<MBNNRDTRdU32OptionalArray>)congestionNumeric;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The distance between each pair of coordinates in meters.
 * @sa https://docs.mapbox.com/api/navigation/directions/#route-leg-object
 */
- (nullable id<MBNNRDTRdF64Array>)distance;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The duration between each pair of coordinates in the route leg in seconds.
 * @sa https://docs.mapbox.com/api/navigation/directions/#route-leg-object
 */
- (nullable id<MBNNRDTRdF64Array>)duration;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The average speed used in the calculation between the two points in each pair of coordinates in meters per second.
 * @sa https://docs.mapbox.com/api/navigation/directions/#route-leg-object
 */
- (nullable id<MBNNRDTRdF64Array>)speed;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The local posted speed limit between each pair of coordinates.
 * @sa https://docs.mapbox.com/api/navigation/directions/#route-leg-object
 */
- (nullable id<MBNNRDTRdSpeedLimitAnnotationArray>)maxspeed;
@end
