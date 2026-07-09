// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPoint3d;
typedef NS_ENUM(NSInteger, MBNNFeatureColor);

NS_SWIFT_NAME(PerceptionFeatureData)
__attribute__((visibility ("default")))
@interface MBNNPerceptionFeatureData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(nullable NSNumber *)id_
                          geometry:(nonnull NSArray<MBNNPoint3d *> *)geometry
                             angle:(nullable NSNumber *)angle
                             color:(nullable NSNumber *)color
                       probability:(nullable NSNumber *)probability
                          accuracy:(nullable NSArray<NSNumber *> *)accuracy
                            extras:(nullable id)extras;

/** Unique tracking identifier, if available. */
@property (nonatomic, readonly, nullable) NSNumber *id;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Geometry representing the feature. Polyline for line features or a single point for point features.
 * The coordinate system must adhere ISO 8855.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNPoint3d *> *geometry;

/**
 * Angle of the feature in degrees, relative to the ego vehicle's heading.
 * If null is provided, angle will be calculated from feature position relative to ego.
 */
@property (nonatomic, readonly, nullable) NSNumber *angle;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Color of the feature, if applicable.
 * This can be used to represent the color of road markings, traffic signs, etc.
 * If null is provided, the color is considered to be FeatureColor#White for road markings.
 */
@property (nonatomic, readonly, nullable) NSNumber *color;

/**
 * Detection probability, [0.0, 1.0] range.
 * If null is provided, the probability is considered to be 1.0 (100% confidence).
 */
@property (nonatomic, readonly, nullable) NSNumber *probability;

/**
 * Estimated positional accuracy for each vertex of the geometry, in meters.
 * This array must be of the same length as the geometry array.
 * A smaller value indicates higher positional confidence (e.g., 0.05 = ±5 cm accuracy).
 * If null is provided, the accuracy is considered to be 0.0 (perfect accuracy).
 */
@property (nonatomic, readonly, nullable, copy) NSArray<NSNumber *> *accuracy;

/** Optional information about feature. @See TrafficSignType#SpeedLimit and RoadMarkingType#SpeedLimitNumber. */
@property (nonatomic, readonly, nullable, copy) id extras;


- (BOOL)isEqualToPerceptionFeatureData:(nonnull MBNNPerceptionFeatureData *)other;

@end
