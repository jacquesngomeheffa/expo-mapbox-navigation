// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRoadObjectDistanceInfo;
typedef NS_ENUM(NSInteger, MBNNRoadObjectType);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Represents the current distance to a road object.
 * The structure is immutable, to get an updated info you should get a new instance of the info
 * via ElectronicHorizonObserver.
 */
NS_SWIFT_NAME(RoadObjectDistance)
__attribute__((visibility ("default")))
@interface MBNNRoadObjectDistance : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithRoadObjectId:(nonnull NSString *)roadObjectId
                                        type:(MBNNRoadObjectType)type
                                distanceInfo:(nonnull MBNNRoadObjectDistanceInfo *)distanceInfo;

/** id of road object */
@property (nonatomic, readonly, nonnull, copy) NSString *roadObjectId;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * type of road object
 */
@property (nonatomic, readonly) MBNNRoadObjectType type;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * distance info
 */
@property (nonatomic, readonly, nonnull) MBNNRoadObjectDistanceInfo *distanceInfo;


@end
