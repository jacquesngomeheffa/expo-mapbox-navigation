// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNAdasisConfigProfilelongTrafficEventIds;

NS_SWIFT_NAME(AdasisConfigTrafficEvent)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigTrafficEvent : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithTrafficEventIds:(nonnull MBNNAdasisConfigProfilelongTrafficEventIds *)trafficEventIds;

- (nonnull instancetype)initWithProfileType:(uint8_t)profileType
                            trafficEventIds:(nonnull MBNNAdasisConfigProfilelongTrafficEventIds *)trafficEventIds
                                     radius:(uint32_t)radius;

/** Set to 0 to disable or use a number in interval [16..31] to send with appropriate sub-type id */
@property (nonatomic, readonly) uint8_t profileType;

@property (nonatomic, readonly, nonnull) MBNNAdasisConfigProfilelongTrafficEventIds *trafficEventIds;
/** Radius(m) from the current position to provide incidents */
@property (nonatomic, readonly) uint32_t radius;


- (BOOL)isEqualToAdasisConfigTrafficEvent:(nonnull MBNNAdasisConfigTrafficEvent *)other;

@end
