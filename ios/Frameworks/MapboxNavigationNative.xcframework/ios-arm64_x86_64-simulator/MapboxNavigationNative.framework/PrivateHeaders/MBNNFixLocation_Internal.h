// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Represents a comprehensive location point with rich metadata.
 * All complex data is stored in and retrieved from the 'extras' map.
 */
NS_SWIFT_NAME(FixLocation)
__attribute__((visibility ("default")))
@interface MBNNFixLocation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithCoordinate:(CLLocationCoordinate2D)coordinate
             monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds
                                      time:(nonnull NSDate *)time
                                     speed:(nullable NSNumber *)speed
                                   bearing:(nullable NSNumber *)bearing
                                  altitude:(nullable NSNumber *)altitude
                        accuracyHorizontal:(nullable NSNumber *)accuracyHorizontal
                                  provider:(nullable NSString *)provider
                           bearingAccuracy:(nullable NSNumber *)bearingAccuracy
                             speedAccuracy:(nullable NSNumber *)speedAccuracy
                          verticalAccuracy:(nullable NSNumber *)verticalAccuracy
                                    extras:(nonnull NSDictionary<NSString *, id> *)extras
                                    isMock:(BOOL)isMock;

/**
 * @brief A coordinate value as defined by mapbox-bindgen (x is lon, y is lat).
 *
 * How to get (examples):
 * - Android: `Location.getLongitude()` and `Location.getLatitude()`
 * - u-blox: `UBX-NAV-PVT` (`lon`, `lat` fields, int32, 1e-7 deg scale).
 */
@property (nonatomic, readonly) CLLocationCoordinate2D coordinate;

/**
 * @brief Monotonic timestamp corresponding to the current location.
 * Measured in nanoseconds. Acquired at the earliest possible point.
 * If the monotonic time is not provided by system on some platforms, then using of non-monotonic clock is allowed.
 * If the value is greater than zero, then getStatus(u64) must be used instead of getStatus(timestamp).
 *
 * How to get (examples):
 * - Host-side timestamp captured by the Driver upon message reception:
 *   - E.g., Android: `Location.getElapsedRealtimeNanos()`
 */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;

/**
 * @brief The absolute wall-clock time of the fix (UTC).
 * A timestamp value as defined by mapbox-bindgen.
 *
 * How to get (examples):
 * - Android: `Location.getTime()` (returns ms), must convert to `system_clock::time_point`.
 * - u-blox: `UBX-NAV-PVT` (fields `year`, `month`, `day`, `hour`, etc.). Driver/HAL must construct this.
 */
@property (nonatomic, readonly, nonnull) NSDate *time;

/**
 * @brief Speed over ground in meters per second.
 *
 * How to get (examples):
 * - Android: `Location.getSpeed()`
 * - u-blox: `UBX-NAV-PVT` (`gSpeed` field, int32, mm/s).
 */
@property (nonatomic, readonly, nullable) NSNumber *speed;

/**
 * @brief Bearing (course over ground) in degrees, 0-360. Optional.
 * 360 is treated as North (i.e. 0).
 * Other values are just ignored (we treat it as if null was passed).
 *
 * How to get (examples):
 * - Android: `Location.getBearing()`
 * - u-blox: `UBX-NAV-PVT` (`headMot` field, int32, 1e-5 deg scale).
 */
@property (nonatomic, readonly, nullable) NSNumber *bearing;

/**
 * @brief The altitude of the device in meters above the WGS84 ellipsoid.
 *
 * How to get (examples):
 * - Android: `Location.getAltitude()`
 * - u-blox: `UBX-NAV-PVT` (`hEll` field, int32, mm).
 */
@property (nonatomic, readonly, nullable) NSNumber *altitude;

/**
 * @brief Estimated horizontal accuracy in meters.
 *
 * How to get (examples):
 * - Android: `Location.getAccuracy()`
 * - u-blox: `UBX-NAV-PVT` (`hAcc` field, uint32, mm).
 */
@property (nonatomic, readonly, nullable) NSNumber *accuracyHorizontal;

/**
 * @brief Name of the location provider (e.g., "gps", "fused", "custom_dr").
 *
 * How to get (examples):
 * - Android: `Location.getProvider()`
 * - u-blox: Driver-defined string (e.g., "u-blox-f9l").
 */
@property (nonatomic, readonly, nullable, copy) NSString *provider;

/**
 * @brief Estimated bearing accuracy in degrees.
 *
 * How to get (examples):
 * - Android: `Location.getBearingAccuracyDegrees()`
 * - u-blox: `UBX-NAV-PVT` (`headAcc` field, uint32, 1e-5 deg scale).
 */
@property (nonatomic, readonly, nullable) NSNumber *bearingAccuracy;

/**
 * @brief Estimated speed accuracy in meters per second.
 *
 * How to get (examples):
 * - Android: `Location.getSpeedAccuracyMetersPerSecond()`
 * - u-blox: `UBX-NAV-PVT` (`sAcc` field, uint32, mm/s).
 */
@property (nonatomic, readonly, nullable) NSNumber *speedAccuracy;

/**
 * @brief Estimated vertical accuracy in meters.
 *
 * How to get (examples):
 * - Android: `Location.getVerticalAccuracyMeters()`
 * - u-blox: `UBX-NAV-PVT` (`vAcc` field, uint32, mm).
 */
@property (nonatomic, readonly, nullable) NSNumber *verticalAccuracy;

/**
 * @brief Any extra, non-standard values from the location provider.
 *
 * This map may contain additional data from the navigation engine.
 * In case, the value has incorrect type, it'll be ignored.
 *
 * Consumers of this struct can check for the following keys:
 *
 * | Key                     | Type (in map)                     | Android Source                                       | u-blox Source                                       |
 * | ----------------------- | --------------------------------- | ---------------------------------------------------- | --------------------------------------------------- |
 * | "positioningMode"       | `uint64_t (as PositioningMode)`   | Can be inferred                                      | `UBX-NAV-PVT` (`fixType`, `flags`)                  |
 * | "integrityStatus"       | `uint64_t (as IntegrityStatus)`   | `Unknown` (Not provided)                             | `UBX-NAV-PL`                                        |
 * | "multipathStatus"       | `uint64_t (as MultipathStatus)`   | Aggregated from `GnssMeasurement.multipathIndicator` | Inferred (e.g., average from `UBX-RXM-MEASX`)       |
 * | "jammingStatus"         | `uint64_t (as JammingStatus)`     | `Unknown` (Not provided)                             | `UBX-SEC-SIG` (`jammingState` + `jamDetEnabled`)    |
 * | "spoofingStatus"        | `uint64_t (as SpoofingStatus)`    | `Unknown` (Not provided)                             | `UBX-SEC-SIG` (`spoofingState` + `spoofDetEnabled`) |
 * | "rawAttitudeYaw"        | `double`                          | `Unknown` (Not provided)                             | `UBX-NAV-ATT` (`heading`)                           |
 * | "rawAttitudePitch"      | `double`                          | `Unknown` (Not provided)                             | `UBX-NAV-ATT` (`pitch`)                             |
 * | "rawAttitudeRoll"       | `double`                          | `Unknown` (Not provided)                             | `UBX-NAV-ATT` (`roll`)                              |
 * | "imuMountYaw"           | `double`                          | `Unknown` (Not provided)                             | `UBX-ESF-ALG` (`yaw`)                               |
 * | "imuMountPitch"         | `double`                          | `Unknown` (Not provided)                             | `UBX-ESF-ALG` (`pitch`)                             |
 * | "imuMountRoll"          | `double`                          | `Unknown` (Not provided)                             | `UBX-ESF-ALG` (`roll`)                              |
 * | "drWheelStatus"         | `uint64_t (as CalibrationStatus)` | `Unknown` (Not provided)                             | `UBX-ESF-STATUS` (`calibStatus` for Wheel)          |
 * | "drImuGyroCalibStatus"  | `uint64_t (as CalibrationStatus)` | `Unknown` (Not provided)                             | `UBX-ESF-STATUS` (`calibStatus` for Gyro)           |
 * | "drImuAccelCalibStatus" | `uint64_t (as CalibrationStatus)` | `Unknown` (Not provided)                             | `UBX-ESF-STATUS` (`calibStatus` for Accel)          |
 * | "drImuStatus"           | `uint64_t (as CalibrationStatus)` | `Unknown` (Not provided)                             | `UBX-ESF-STATUS` (`mntAlgStatus` + `fusionMode`)    |
 * | "drFusionStatus"        | `uint64_t (as CalibrationStatus)` | `Unknown` (Not provided)                             | `UBX-ESF-STATUS` (`fusionMode`)                     |
 *
 * Note: at the moment additional fields are used (will be REMOVED soon):
 *   - `isDR: boolean` - should be `true` if this location is a result of dead reckoning
 *                       and can be potentially not as precise as usually, used on automotive profile only
 *                       (will be replaced with `positioningMode`).
 *   - `satellites: int` - corresponds to the number of satellites that were used
 *                         to identify the current location. Applicable to automotive profile only
 *                         (will be inferred from `RawGnssData` interface).
 */
@property (nonatomic, readonly, nonnull, copy) NSDictionary<NSString *, id> *extras;

/**
 * @brief True if this location was generated by a mock/test provider.
 * If this value is `true` - teleport telemetry event will not be sent
 * (to keep telemetry events clean from repeated fake mock-provided events)
 *
 * How to get (examples):
 * - Android: `Location.isFromMockProvider()`
 * - u-blox: `false`, unless the driver is simulating data.
 */
@property (nonatomic, readonly) BOOL isMock;


- (BOOL)isEqualToFixLocation:(nonnull MBNNFixLocation *)other;

@end
