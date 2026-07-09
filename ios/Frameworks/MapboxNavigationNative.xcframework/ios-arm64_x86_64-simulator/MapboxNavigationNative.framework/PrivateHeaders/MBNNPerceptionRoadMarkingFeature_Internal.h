// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPerceptionFeatureData;
typedef NS_ENUM(NSInteger, MBNNRoadMarkingType);

NS_SWIFT_NAME(PerceptionRoadMarkingFeature)
__attribute__((visibility ("default")))
@interface MBNNPerceptionRoadMarkingFeature : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithType:(MBNNRoadMarkingType)type
                                data:(nonnull MBNNPerceptionFeatureData *)data;

@property (nonatomic, readonly) MBNNRoadMarkingType type;
@property (nonatomic, readonly, nonnull) MBNNPerceptionFeatureData *data;

- (BOOL)isEqualToPerceptionRoadMarkingFeature:(nonnull MBNNPerceptionRoadMarkingFeature *)other;

@end
