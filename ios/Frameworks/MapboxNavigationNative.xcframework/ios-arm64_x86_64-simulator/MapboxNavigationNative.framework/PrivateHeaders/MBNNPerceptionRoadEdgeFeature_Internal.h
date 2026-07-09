// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPerceptionFeatureData;
typedef NS_ENUM(NSInteger, MBNNRoadEdgeType);

NS_SWIFT_NAME(PerceptionRoadEdgeFeature)
__attribute__((visibility ("default")))
@interface MBNNPerceptionRoadEdgeFeature : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithType:(MBNNRoadEdgeType)type
                                data:(nonnull MBNNPerceptionFeatureData *)data;

@property (nonatomic, readonly) MBNNRoadEdgeType type;
@property (nonatomic, readonly, nonnull) MBNNPerceptionFeatureData *data;

- (BOOL)isEqualToPerceptionRoadEdgeFeature:(nonnull MBNNPerceptionRoadEdgeFeature *)other;

@end
