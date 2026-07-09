// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNLaneAccessability;
typedef NS_ENUM(NSInteger, MBNNLaneIndication);

NS_SWIFT_NAME(TurnLane)
__attribute__((visibility ("default")))
@interface MBNNTurnLane : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithValid:(BOOL)valid
                               active:(BOOL)active
                      validIndication:(nullable NSNumber *)validIndication
                          indications:(nonnull NSArray<NSNumber *> *)indications
                               access:(nullable MBNNLaneAccessability *)access;

/** Indicates whether a lane can be used to complete the maneuver (true) or not (false). */
@property (nonatomic, readonly) BOOL valid;

/** Indicates whether a lane is a preferred lane (true) or not (false). A preferred lane is a lane that is recommended if there are multiple lanes available. */
@property (nonatomic, readonly) BOOL active;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * When either the valid or active value is true, this property shows which of the lane indications applies to the current route, when there is more than one.
 */
@property (nonatomic, readonly, nullable) NSNumber *validIndication;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The indications (based on signs, road markings, or both) for a lane. A road can have multiple indications.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *indications;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Describes lane accessability data
 */
@property (nonatomic, readonly, nullable) MBNNLaneAccessability *access;


- (BOOL)isEqualToTurnLane:(nonnull MBNNTurnLane *)other;

@end
