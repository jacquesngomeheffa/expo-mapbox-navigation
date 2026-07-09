// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNPerceptionFeatureData;
typedef NS_ENUM(NSInteger, MBNNTrafficSignType);

NS_SWIFT_NAME(PerceptionTrafficSignFeature)
__attribute__((visibility ("default")))
@interface MBNNPerceptionTrafficSignFeature : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithType:(MBNNTrafficSignType)type
                                data:(nonnull MBNNPerceptionFeatureData *)data;

@property (nonatomic, readonly) MBNNTrafficSignType type;
@property (nonatomic, readonly, nonnull) MBNNPerceptionFeatureData *data;

- (BOOL)isEqualToPerceptionTrafficSignFeature:(nonnull MBNNPerceptionTrafficSignFeature *)other;

@end
