// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNRDTRdLaneIndication);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Contains details for a route step lane object.
 *
 * \sa https://docs.mapbox.com/api/navigation/directions/#lane-object
 */
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

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * This property shows which of the lane indications is applicable to the current route,
 *  when there is more than one. For example, if a lane allows you to go left or straight but
 *  your current route is guiding you to the left, then this value will be set to left.
 */
@property (nonatomic, readonly, nullable) NSNumber *validIndication;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * An array of indications (based on signs, road markings, or both) for each turn lane.
 *  A road can have multiple indications. For example, a turn lane can have a sign with
 *  an arrow pointing left and another arrow pointing straight.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<NSNumber *> *indications;


@end
