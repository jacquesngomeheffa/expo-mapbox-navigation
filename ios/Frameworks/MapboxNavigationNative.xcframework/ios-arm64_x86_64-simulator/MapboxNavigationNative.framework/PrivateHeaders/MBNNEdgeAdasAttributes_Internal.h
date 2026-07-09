// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRoadItemOnEdge;
@class MBNNSpeedLimitInfo;
@class MBNNValueOnEdge;
typedef NS_ENUM(NSInteger, MBNNETC2RoadType);
typedef NS_ENUM(NSInteger, MBNNFormOfWay);

NS_SWIFT_NAME(EdgeAdasAttributes)
__attribute__((visibility ("default")))
@interface MBNNEdgeAdasAttributes : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithSpeedLimit:(nonnull NSArray<MBNNSpeedLimitInfo *> *)speedLimit
                                    slopes:(nonnull NSArray<MBNNValueOnEdge *> *)slopes
                                elevations:(nonnull NSArray<MBNNValueOnEdge *> *)elevations
                                curvatures:(nonnull NSArray<MBNNValueOnEdge *> *)curvatures
                             isDividedRoad:(nullable NSNumber *)isDividedRoad
                             isBuiltUpArea:(nullable NSNumber *)isBuiltUpArea
                                 formOfWay:(nullable NSNumber *)formOfWay
                                      etc2:(MBNNETC2RoadType)etc2
                                 roadItems:(nonnull NSArray<MBNNRoadItemOnEdge *> *)roadItems;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * List of speed limits on the edge.
 *  Empty means no speed-limit data for the edge.
 *  Multiple values will have different conditions.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNSpeedLimitInfo *> *speedLimit;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * List of slope values with their positions on the edge.
 * Position is a shape index, where integer part in an index of geometry segment is
 * and fractional part is a position on the segment.
 * Value is a slope in degrees.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNValueOnEdge *> *slopes;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * List of elevation values with their positions on the edge.
 * Position is a shape index, where integer part in an index of geometry segment is
 * and fractional part is a position on the segment.
 * Value is an elevation in meters above sea level.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNValueOnEdge *> *elevations;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * List of curvature values with their positions on the edge.
 * Position is a shape index, where integer part in an index of geometry segment is
 * and fractional part is a position on the segment.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNValueOnEdge *> *curvatures;

/** A flag indicating if the edge is a divided road. */
@property (nonatomic, readonly, nullable) NSNumber *isDividedRoad;

/** A flag indicating if the edge is a built up area. */
@property (nonatomic, readonly, nullable) NSNumber *isBuiltUpArea;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *  Form Of Way information from ADAS tiles, may differ from the Valhalla value, but should be used for ADAS purposes.
 *  If not set, then the value is not known.
 */
@property (nonatomic, readonly, nullable) NSNumber *formOfWay;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *  Road class in ETC2.0 format (Japan specific)
 */
@property (nonatomic, readonly) MBNNETC2RoadType etc2;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Road Items
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRoadItemOnEdge *> *roadItems;


- (BOOL)isEqualToEdgeAdasAttributes:(nonnull MBNNEdgeAdasAttributes *)other;

@end
