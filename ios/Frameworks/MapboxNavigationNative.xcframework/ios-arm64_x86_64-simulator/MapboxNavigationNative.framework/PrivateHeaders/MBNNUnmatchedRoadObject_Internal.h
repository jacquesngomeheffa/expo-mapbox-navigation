// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNUnmatchedRoadObjectGeometry;
typedef NS_ENUM(NSInteger, MBNNUnmatchedRoadObjectGeometryKind);

NS_SWIFT_NAME(UnmatchedRoadObject)
__attribute__((visibility ("default")))
@interface MBNNUnmatchedRoadObject : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(nonnull NSString *)id_
                          geometry:(nonnull MBNNUnmatchedRoadObjectGeometry *)geometry
                      geometryKind:(MBNNUnmatchedRoadObjectGeometryKind)geometryKind
                           heading:(nullable NSNumber *)heading;

/** Id of the resulting object */
@property (nonatomic, readonly, nonnull, copy) NSString *id;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Geometry to be matched
 */
@property (nonatomic, readonly, nonnull) MBNNUnmatchedRoadObjectGeometry *geometry;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Geometry kind
 */
@property (nonatomic, readonly) MBNNUnmatchedRoadObjectGeometryKind geometryKind;

/**
 * Optional heading in degrees from the North. Used for UnmatchedRoadObjectGeometryKind::Point
 * Describes the direction of riding for the edge where provided point is going to be matched
 */
@property (nonatomic, readonly, nullable) NSNumber *heading;


@end
