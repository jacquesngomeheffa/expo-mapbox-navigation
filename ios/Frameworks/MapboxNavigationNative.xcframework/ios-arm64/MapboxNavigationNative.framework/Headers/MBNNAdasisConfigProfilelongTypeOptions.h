// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNAdasisConfigCustomTrafficEvents;
@class MBNNAdasisConfigLonLat;
@class MBNNAdasisConfigProfilelongTrafficSigns;

/** PROFILE LONG message options */
NS_SWIFT_NAME(AdasisConfigProfilelongTypeOptions)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigProfilelongTypeOptions : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithLonLat:(nonnull MBNNAdasisConfigLonLat *)lonLat
                          trafficSigns:(nonnull MBNNAdasisConfigProfilelongTrafficSigns *)trafficSigns
                          customEvents:(nonnull MBNNAdasisConfigCustomTrafficEvents *)customEvents;

@property (nonatomic, readonly, nonnull) MBNNAdasisConfigLonLat *lonLat;
@property (nonatomic, readonly, nonnull) MBNNAdasisConfigProfilelongTrafficSigns *trafficSigns;
@property (nonatomic, readonly, nonnull) MBNNAdasisConfigCustomTrafficEvents *customEvents;

- (BOOL)isEqualToAdasisConfigProfilelongTypeOptions:(nonnull MBNNAdasisConfigProfilelongTypeOptions *)other;

@end
