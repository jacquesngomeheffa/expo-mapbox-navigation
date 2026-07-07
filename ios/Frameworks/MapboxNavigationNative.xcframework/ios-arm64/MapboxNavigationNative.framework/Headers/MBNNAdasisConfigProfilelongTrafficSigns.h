// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNAdasisConfigProfilelongTrafficSignIds;

NS_SWIFT_NAME(AdasisConfigProfilelongTrafficSigns)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigProfilelongTrafficSigns : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithTrafficSignIds:(nonnull MBNNAdasisConfigProfilelongTrafficSignIds *)trafficSignIds;

- (nonnull instancetype)initWithProfileType:(uint8_t)profileType
                             trafficSignIds:(nonnull MBNNAdasisConfigProfilelongTrafficSignIds *)trafficSignIds
                  customConditionForTrailer:(uint8_t)customConditionForTrailer;

/** Set to 0 to disable or use 8 as standard sub-type id */
@property (nonatomic, readonly) uint8_t profileType;

@property (nonatomic, readonly, nonnull) MBNNAdasisConfigProfilelongTrafficSignIds *trafficSignIds;
@property (nonatomic, readonly) uint8_t customConditionForTrailer;

- (BOOL)isEqualToAdasisConfigProfilelongTrafficSigns:(nonnull MBNNAdasisConfigProfilelongTrafficSigns *)other;

@end
