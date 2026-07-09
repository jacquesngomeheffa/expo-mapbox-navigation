// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPerceptionFeatureData;
typedef NS_ENUM(NSInteger, MBNNLaneMarkingType);

NS_SWIFT_NAME(PerceptionLaneMarkingFeature)
__attribute__((visibility ("default")))
@interface MBNNPerceptionLaneMarkingFeature : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithType:(MBNNLaneMarkingType)type
                                data:(nonnull MBNNPerceptionFeatureData *)data;

@property (nonatomic, readonly) MBNNLaneMarkingType type;
@property (nonatomic, readonly, nonnull) MBNNPerceptionFeatureData *data;

- (BOOL)isEqualToPerceptionLaneMarkingFeature:(nonnull MBNNPerceptionLaneMarkingFeature *)other;

@end
