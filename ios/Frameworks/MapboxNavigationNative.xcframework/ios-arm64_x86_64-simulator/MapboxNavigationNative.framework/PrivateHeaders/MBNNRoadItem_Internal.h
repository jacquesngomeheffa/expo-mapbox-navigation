// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNRoadItemLocation);
typedef NS_ENUM(NSInteger, MBNNRoadItemType);

NS_SWIFT_NAME(RoadItem)
__attribute__((visibility ("default")))
@interface MBNNRoadItem : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithType:(MBNNRoadItemType)type
                            location:(nullable NSNumber *)location
                               lanes:(nonnull NSArray<NSNumber *> *)lanes
                               value:(nullable NSNumber *)value;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The type of the road item.
 */
@property (nonatomic, readonly) MBNNRoadItemType type;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The location of the road item: Right, Left, Above, OnSurface, or Above a specific lane
 */
@property (nonatomic, readonly, nullable) NSNumber *location;

/** The lanes of the road item, enabled only if location is AboveLane */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *lanes;

/**
 * The value on the road sign. Enabled only for types RoadItemType.SpeedLimitSign,
 * RoadItemType.RoadCam*,  RoadItemType.SteepAscentSign, RoadItemType.SteepDescentSign
 */
@property (nonatomic, readonly, nullable) NSNumber *value;


- (BOOL)isEqualToRoadItem:(nonnull MBNNRoadItem *)other;

@end
