// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPerceptionLaneMarkingFeature;
@class MBNNPerceptionRoadEdgeFeature;
@class MBNNPerceptionRoadMarkingFeature;
@class MBNNPerceptionTrafficSignFeature;
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNPerceptionFeatureType)
{
    MBNNPerceptionFeatureTypePerceptionLaneMarkingFeature,
    MBNNPerceptionFeatureTypePerceptionRoadEdgeFeature,
    MBNNPerceptionFeatureTypePerceptionRoadMarkingFeature,
    MBNNPerceptionFeatureTypePerceptionTrafficSignFeature
} NS_SWIFT_NAME(PerceptionFeatureType);

NS_SWIFT_NAME(PerceptionFeature)
__attribute__((visibility ("default")))
@interface MBNNPerceptionFeature : NSObject

// This class provides factory method which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides factory method which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

+ (nonnull instancetype)fromPerceptionLaneMarkingFeature:(nonnull MBNNPerceptionLaneMarkingFeature *)value;
+ (nonnull instancetype)fromPerceptionRoadEdgeFeature:(nonnull MBNNPerceptionRoadEdgeFeature *)value;
+ (nonnull instancetype)fromPerceptionRoadMarkingFeature:(nonnull MBNNPerceptionRoadMarkingFeature *)value;
+ (nonnull instancetype)fromPerceptionTrafficSignFeature:(nonnull MBNNPerceptionTrafficSignFeature *)value;

- (BOOL)isPerceptionLaneMarkingFeature;
- (BOOL)isPerceptionRoadEdgeFeature;
- (BOOL)isPerceptionRoadMarkingFeature;
- (BOOL)isPerceptionTrafficSignFeature;

- (nonnull MBNNPerceptionLaneMarkingFeature *)getPerceptionLaneMarkingFeature __attribute((ns_returns_retained));
- (nonnull MBNNPerceptionRoadEdgeFeature *)getPerceptionRoadEdgeFeature __attribute((ns_returns_retained));
- (nonnull MBNNPerceptionRoadMarkingFeature *)getPerceptionRoadMarkingFeature __attribute((ns_returns_retained));
- (nonnull MBNNPerceptionTrafficSignFeature *)getPerceptionTrafficSignFeature __attribute((ns_returns_retained));

@property (nonatomic, readonly) MBNNPerceptionFeatureType type;

@end
