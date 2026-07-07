// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNRDTRdLaneIndication_Internal.h>

NS_SWIFT_NAME(RdLane)
__attribute__((visibility ("default")))
@interface MBNNRDTRdLane : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithValid:(BOOL)valid
                               active:(BOOL)active
                      validIndication:(nullable NSNumber *)validIndication
                          indications:(nonnull NSArray<NSNumber *> *)indications;

/** Indicates whether a lane can be taken to complete the maneuver or not. */
@property (nonatomic, readonly) BOOL valid;

/**
 * Indicates whether this lane is a preferred lane or not.
 *  A preferred lane is a lane that is recommended if there are multiple lanes available.
 */
@property (nonatomic, readonly) BOOL active;

@property (nonatomic, readonly, nullable) NSNumber *validIndication;
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *indications;

@end
