// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNDetectionState;
@class MBNNPoint3d;
typedef NS_ENUM(NSInteger, MBNNDetectedObjectCategory);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Structure containing raw information about detected object
 */
NS_SWIFT_NAME(DetectedObject)
__attribute__((visibility ("default")))
@interface MBNNDetectedObject : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(uint64_t)id_
                              type:(nonnull NSString *)type
                          category:(MBNNDetectedObjectCategory)category
                             state:(nullable MBNNDetectionState *)state
                            center:(nonnull MBNNPoint3d *)center
                           bearing:(nullable NSNumber *)bearing
                             speed:(nullable NSNumber *)speed
                        confidence:(nullable NSNumber *)confidence
                        dimensions:(nullable NSArray<NSNumber *> *)dimensions;

/** Id of the detected object */
@property (nonatomic, readonly) uint64_t id;

/** Type of the object. A string matching an entry with the same key to select the model for the object type. */
@property (nonatomic, readonly, nonnull, copy) NSString *type;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Category of the object.
 */
@property (nonatomic, readonly) MBNNDetectedObjectCategory category;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Supplementary state of the detection.
 */
@property (nonatomic, readonly, nullable) MBNNDetectionState *state;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Center of the detected object. The coordinate system must adhere ISO 8855.
 */
@property (nonatomic, readonly, nonnull) MBNNPoint3d *center;

/** The ego-relative bearing of the object, in the 0, 360 degree range. */
@property (nonatomic, readonly, nullable) NSNumber *bearing;

/**
 * Absolute scalar speed of the object in meters per second.
 *
 * This value is always non-negative and represents the magnitude of the object's velocity vector.
 * Direction of motion is given separately via the `bearing` field, which is ego-relative.
 */
@property (nonatomic, readonly, nullable) NSNumber *speed;

/**
 * The confidence score of the detection, ranging from 0.0 to 1.0.
 * A higher value indicates a more reliable detection.
 */
@property (nonatomic, readonly, nullable) NSNumber *confidence;

/**
 * Dimensions of the detected object, in meters.
 * Represents either a 2D or 3D bounding box:
 * - For 2D: [width, length]
 * - For 3D: [width, length, height]
 *
 * The array must contain either 2 or 3 elements.
 */
@property (nonatomic, readonly, nullable, copy) NSArray<NSNumber *> *dimensions;


- (BOOL)isEqualToDetectedObject:(nonnull MBNNDetectedObject *)other;

@end
