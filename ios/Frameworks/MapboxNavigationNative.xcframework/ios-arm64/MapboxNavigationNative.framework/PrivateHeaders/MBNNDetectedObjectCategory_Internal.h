// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Category of vehicle for styling and filtering purposes
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNDetectedObjectCategory)
{
    /** Vehicles such as cars, buses, trucks, vans, etc. */
    MBNNDetectedObjectCategoryVehicle,
    /** Two-wheeler vehicles such as bicycles, motorcycles, scooters, etc. */
    MBNNDetectedObjectCategoryTwoWheeler,
    /** Pedestrian objects. */
    MBNNDetectedObjectCategoryPedestrian,
    /** Static objects such as traffic cones, lamp posts, traffic signs, etc. */
    MBNNDetectedObjectCategoryStatic
} NS_SWIFT_NAME(DetectedObjectCategory);

NSString* MBNNDetectedObjectCategoryToString(MBNNDetectedObjectCategory detected_object_category);
