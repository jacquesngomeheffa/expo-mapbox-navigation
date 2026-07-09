// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNConstellationType);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Contains raw measurement data for a single GNSS satellite.
 *
 * This struct primarily represents data from a "satellite status" message.
 * - Android: `GnssStatus` object from `GnssStatus.Callback`.
 * - u-blox: `UBX-NAV-SAT` message's repeating block.
 */
NS_SWIFT_NAME(RawGnssSatelliteData)
__attribute__((visibility ("default")))
@interface MBNNRawGnssSatelliteData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithSvid:(int32_t)svid
                  carrierFrequencyHz:(nullable NSNumber *)carrierFrequencyHz
                     basebandCn0DbHz:(nullable NSNumber *)basebandCn0DbHz
                             cn0DbHz:(nullable NSNumber *)cn0DbHz
                           usedInFix:(nullable NSNumber *)usedInFix
                    hasEphemerisData:(nullable NSNumber *)hasEphemerisData
                      hasAlmanacData:(nullable NSNumber *)hasAlmanacData
                   constellationType:(nullable NSNumber *)constellationType
                      azimuthDegrees:(nullable NSNumber *)azimuthDegrees
                    elevationDegrees:(nullable NSNumber *)elevationDegrees
                            residual:(nullable NSNumber *)residual;

/**
 * @brief The satellite identifier (PRN or SVID).
 *
 * How to get (examples):
 * - Android: `GnssStatus.getSvid(i)`
 * - u-blox: `UBX-NAV-SAT` (`svId` field)
 */
@property (nonatomic, readonly) int32_t svid;

/**
 * @brief The carrier frequency of the tracked signal in Hz.
 *
 * How to get (examples):
 * - Android: `GnssStatus.getCarrierFrequencyHz(i)` (if `hasCarrierFrequencyHz()` is true)
 * - u-blox: `UBX-RXM-RAWX` (`sigId` field, requires mapping)
 */
@property (nonatomic, readonly, nullable) NSNumber *carrierFrequencyHz;

/**
 * @brief The baseband carrier-to-noise density in dB-Hz.
 *
 * How to get (examples):
 * - Android: `GnssMeasurement.getBasebandCn0DbHz()` (from `GnssMeasurementEvent`)
 * - u-blox: Not directly available. Related to `UBX-MON-RF` (`noisePerMS`)
 */
@property (nonatomic, readonly, nullable) NSNumber *basebandCn0DbHz;

/**
 * @brief The carrier-to-noise density in dB-Hz.
 *
 * How to get (examples):
 * - Android: `GnssStatus.getCn0DbHz(i)`
 * - u-blox: `UBX-NAV-SAT` (`cno` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *cn0DbHz;

/**
 * @brief True if the satellite was used in the most recent position fix.
 *
 * How to get (examples):
 * - Android: `GnssStatus.usedInFix(i)`
 * - u-blox: `UBX-NAV-SAT` (`flags` field, bit 0 `svUsed`)
 */
@property (nonatomic, readonly, nullable) NSNumber *usedInFix;

/**
 * @brief True if the receiver has ephemeris data for this satellite.
 *
 * How to get (examples):
 * - Android: `GnssStatus.getSvid(i)`
 * - u-blox: `UBX-NAV-SAT` (`flags` field, bit 3 `ephAvail`)
 */
@property (nonatomic, readonly, nullable) NSNumber *hasEphemerisData;

/**
 * @brief True if the receiver has almanac data for this satellite.
 *
 * How to get (examples):
 * - Android: `GnssStatus.hasAlmanacData(i)`
 * - u-blox: `UBX-NAV-SAT` (`flags` field, bit 4 `almAvail`)
 */
@property (nonatomic, readonly, nullable) NSNumber *hasAlmanacData;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Constellation type of the satellite.
 *
 * How to get (examples):
 * - Android: `GnssStatus.getConstellationType(i)` (requires mapping to this enum)
 * - u-blox: `UBX-NAV-SAT` (`gnssId` field, requires mapping to this enum)
 */
@property (nonatomic, readonly, nullable) NSNumber *constellationType;

/**
 * @brief The azimuth of the satellite in degrees (0-359.99).
 *
 * How to get (examples):
 * - Android: `GnssStatus.getAzimuthDegrees(i)` (if `hasAlmanacData()` is true)
 * - u-blox: `UBX-NAV-SAT` (`azim` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *azimuthDegrees;

/**
 * @brief The elevation of the satellite in degrees (0-90).
 *
 * How to get (examples):
 * - Android: `GnssStatus.getElevationDegrees(i)` (if `hasAlmanacData()` is true)
 * - u-blox: `UBX-NAV-SAT` (`elev` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *elevationDegrees;

/**
 * @brief The pseudorange residual in meters.
 *
 * How to get (examples):
 * - Android: `GnssMeasurement.getPseudorangeResidualMeters()` (from `GnssMeasurementEvent`)
 * - u-blox: `UBX-NAV-SAT` (`prRes` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *residual;


- (BOOL)isEqualToRawGnssSatelliteData:(nonnull MBNNRawGnssSatelliteData *)other;

@end
