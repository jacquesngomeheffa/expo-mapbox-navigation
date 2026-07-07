// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <MapboxNavigationNative/MBNNRDTRdManeuverModifier_Internal.h>
#import <MapboxNavigationNative/MBNNRDTRdManeuverType_Internal.h>

NS_SWIFT_NAME(RdManeuver)
__attribute__((visibility ("default")))
@interface MBNNRDTRdManeuver : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithBearingBefore:(float)bearingBefore
                                 bearingAfter:(float)bearingAfter
                                  instruction:(nonnull NSString *)instruction
                                     location:(CLLocationCoordinate2D)location
                               roundaboutExit:(nullable NSNumber *)roundaboutExit
                                      degrees:(nullable NSNumber *)degrees;

- (nonnull instancetype)initWithBearingBefore:(float)bearingBefore
                                 bearingAfter:(float)bearingAfter
                                  instruction:(nonnull NSString *)instruction
                                     location:(CLLocationCoordinate2D)location
                                     modifier:(MBNNRDTRdManeuverModifier)modifier
                                         type:(MBNNRDTRdManeuverType)type
                               roundaboutExit:(nullable NSNumber *)roundaboutExit
                                      degrees:(nullable NSNumber *)degrees;

/**
 * A number between 0 and 360 indicating the clockwise angle from true north to the direction of
 * travel immediately before the maneuver
 */
@property (nonatomic, readonly) float bearingBefore;

/**
 * A number between 0 and 360 indicating the clockwise angle from true north to the direction of
 * travel immediately after the maneuver.
 */
@property (nonatomic, readonly) float bearingAfter;

/** A human-readable instruction of how to execute the returned maneuver. */
@property (nonatomic, readonly, nonnull, copy) NSString *instruction;

/** Coordinates for the point of the maneuver. */
@property (nonatomic, readonly) CLLocationCoordinate2D location;

@property (nonatomic, readonly) MBNNRDTRdManeuverModifier modifier;
@property (nonatomic, readonly) MBNNRDTRdManeuverType type;
/** A number of roundabout exit */
@property (nonatomic, readonly, nullable) NSNumber *roundaboutExit;

/**
 * The degrees at which you will be exiting a roundabout,
 * assuming 180 indicates going straight through the roundabout.
 */
@property (nonatomic, readonly, nullable) NSNumber *degrees;


@end
