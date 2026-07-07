// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRoadObjectType_Internal.h>

@class MBNNRoadObjectDistanceInfo;

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

@property (nonatomic, readonly) MBNNRoadObjectType type;
@property (nonatomic, readonly, nonnull) MBNNRoadObjectDistanceInfo *distanceInfo;

@end
