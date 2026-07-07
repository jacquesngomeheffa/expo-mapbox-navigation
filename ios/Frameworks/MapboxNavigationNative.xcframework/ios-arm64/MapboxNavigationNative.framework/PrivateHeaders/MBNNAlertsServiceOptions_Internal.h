// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

NS_SWIFT_NAME(AlertsServiceOptions)
__attribute__((visibility ("default")))
@interface MBNNAlertsServiceOptions : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithCollectTunnels:(nullable NSNumber *)collectTunnels
                                collectBridges:(nullable NSNumber *)collectBridges
                        collectRestrictedAreas:(nullable NSNumber *)collectRestrictedAreas
                           collectMergingAreas:(nullable NSNumber *)collectMergingAreas
                           collectServiceAreas:(nullable NSNumber *)collectServiceAreas;

@property (nonatomic, readonly, nullable) NSNumber *collectTunnels;
@property (nonatomic, readonly, nullable) NSNumber *collectBridges;
@property (nonatomic, readonly, nullable) NSNumber *collectRestrictedAreas;
@property (nonatomic, readonly, nullable) NSNumber *collectMergingAreas;
@property (nonatomic, readonly, nullable) NSNumber *collectServiceAreas;

@end
