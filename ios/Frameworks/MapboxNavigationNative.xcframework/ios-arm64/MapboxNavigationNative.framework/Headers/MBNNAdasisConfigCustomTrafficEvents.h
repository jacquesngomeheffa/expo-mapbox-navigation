// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNAdasisConfigTrafficEvent;
@class MBNNAdasisConfigWeather;

NS_SWIFT_NAME(AdasisConfigCustomTrafficEvents)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigCustomTrafficEvents : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithTrafficEvent:(nonnull MBNNAdasisConfigTrafficEvent *)trafficEvent
                                     weather:(nonnull MBNNAdasisConfigWeather *)weather;

@property (nonatomic, readonly, nonnull) MBNNAdasisConfigTrafficEvent *trafficEvent;
@property (nonatomic, readonly, nonnull) MBNNAdasisConfigWeather *weather;

- (BOOL)isEqualToAdasisConfigCustomTrafficEvents:(nonnull MBNNAdasisConfigCustomTrafficEvents *)other;

@end
