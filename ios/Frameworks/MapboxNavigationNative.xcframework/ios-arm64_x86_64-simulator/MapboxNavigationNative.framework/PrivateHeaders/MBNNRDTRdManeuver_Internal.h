// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

typedef NS_ENUM(NSInteger, MBNNRDTRdManeuverModifier);
typedef NS_ENUM(NSInteger, MBNNRDTRdManeuverType);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Contains details for a route step maneuver.
 *
 * \sa https://docs.mapbox.com/api/navigation/#step-maneuver-object
 */
NS_SWIFT_NAME(RdManeuver)
__attribute__((visibility ("default")))
@interface MBNNRDTRdManeuver : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithBearingBefore:(uint16_t)bearingBefore
                                 bearingAfter:(uint16_t)bearingAfter
                                  instruction:(nonnull NSString *)instruction
                                     location:(CLLocationCoordinate2D)location
                                     modifier:(nullable NSNumber *)modifier
                                         type:(MBNNRDTRdManeuverType)type;

/**
 * A number between 0 and 360 indicating the clockwise angle from true north to the direction of
 * travel immediately before the maneuver
 */
@property (nonatomic, readonly) uint16_t bearingBefore;

/**
 * A number between 0 and 360 indicating the clockwise angle from true north to the direction of
 * travel immediately after the maneuver.
 */
@property (nonatomic, readonly) uint16_t bearingAfter;

/** A human-readable instruction of how to execute the returned maneuver. */
@property (nonatomic, readonly, nonnull, copy) NSString *instruction;

/** Coordinates for the point of the maneuver. */
@property (nonatomic, readonly) CLLocationCoordinate2D location;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Direction change of the maneuver. The meaning of each
 * modifier depends on the \ref type property.
 */
@property (nonatomic, readonly, nullable) NSNumber *modifier;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Type of maneuver.
 * \sa https://docs.mapbox.com/api/navigation/#maneuver-types
 */
@property (nonatomic, readonly) MBNNRDTRdManeuverType type;


@end
