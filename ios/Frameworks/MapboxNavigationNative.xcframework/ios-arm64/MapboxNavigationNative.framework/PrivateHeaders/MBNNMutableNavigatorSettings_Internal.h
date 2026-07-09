// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNTestingContext;
typedef NS_ENUM(NSInteger, MBNNVehicleType);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Navigator settings that can be changed in runtime
 */
NS_SWIFT_NAME(MutableNavigatorSettings)
@protocol MBNNMutableNavigatorSettings
/**
 * Get number of seconds that should pass before any new proposed maneuver.
 * See MutableNavigatorSettings.setAvoidManeuverSeconds.
 */
- (nullable NSNumber *)avoidManeuverSeconds;
/**
 * Set number of seconds that should pass before any new proposed maneuver.
 * Number of seconds that should pass for safety reasons before any new proposed maneuver.
 * This value is used to calculate safe distance at the moment of request, hence actual timing may
 * differ due to e.g. significant current speed change during specified period. See Directions API
 * avoid_maneuver_radius parameter for details.
 * If unset or set to 0 then new maneuvers are not filtered out on proximity basis.
 * Note, that changing this property is subject of data races: there is no strong guarantee about
 * exact moment of time when changed property value comes into effect.
 */
- (void)setAvoidManeuverSecondsForSeconds:(nullable NSNumber *)seconds;
/** User's preferable languages set to navigator. See `setUserLanguages(...)`. */
- (nonnull NSArray<NSString *> *)userLanguages;
/**
 * User's preferable languages in order of preferences.
 * Contains IETF BCP 47 language tags
 */
- (void)setUserLanguagesForLanguages:(nonnull NSArray<NSString *> *)languages;
/** First of the user's preferable languages supported by Valhalla. See `setUserLanguages(...)`. */
- (nonnull NSString *)userLanguageSupportedByValhalla;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Update the testing context and write "SetTestingContext" event to history if it is
 * changed.
 *
 * @param testingContext Setting this value to empty optional clears the current testing context and
 * records an "empty" "SetTestingContext" event in the history.
 */
- (void)setTestingContextForTestingContext:(nullable MBNNTestingContext *)testingContext;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Retrieves the current testing context information.
 */
- (nullable MBNNTestingContext *)testingContext;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Update type of the vehicle
 */
- (void)setVehicleTypeForVehicleType:(MBNNVehicleType)vehicleType;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Retrieves the current vehicle type.
 */
- (MBNNVehicleType)vehicleType;
@end
