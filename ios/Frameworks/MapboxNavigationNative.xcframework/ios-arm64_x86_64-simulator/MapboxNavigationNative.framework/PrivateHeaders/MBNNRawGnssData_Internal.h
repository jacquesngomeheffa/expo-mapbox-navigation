// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRawGnssSatelliteData;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Contains a snapshot of raw GNSS measurements from the receiver.
 *
 * This struct represents a summary of satellite status and DOP information.
 *  - Android: Aggregated from `GnssStatus.Callback` and `OnNmeaMessageListener`.
 *  - u-blox: Aggregated from `UBX-NAV-SAT` and `UBX-NAV-DOP` messages.
 *
 * Note:
 *  - Dilution of Precision (DOP) - an indicator of satellite geometry used to determine a position.
 *                                  Lower DOPs indicate better geometry, thus more accurate positioning information.
 */
NS_SWIFT_NAME(RawGnssData)
__attribute__((visibility ("default")))
@interface MBNNRawGnssData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithGdop:(nullable NSNumber *)gdop
                                pdop:(nullable NSNumber *)pdop
                                tdop:(nullable NSNumber *)tdop
                                vdop:(nullable NSNumber *)vdop
                                hdop:(nullable NSNumber *)hdop
                                ndop:(nullable NSNumber *)ndop
                                edop:(nullable NSNumber *)edop
                          satellites:(nonnull NSArray<MBNNRawGnssSatelliteData *> *)satellites
       monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds;

/**
 * @brief Geometric dilution of precision.
 *
 * How to get (examples):
 * - Android: NMEA `$GPGGA` (field 8, if available) or `$GPGSA` (field 17, as PDOP)
 * - u-blox: `UBX-NAV-DOP` (`gDOP` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *gdop;

/**
 * @brief Position dilution of precision.
 *
 * How to get (examples):
 * - Android: NMEA `$GPGSA` (field 17)
 * - u-blox: `UBX-NAV-DOP` (`pDOP` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *pdop;

/**
 * @brief Time dilution of precision.
 *
 * How to get (examples):
 * - Android: Not typically available in NMEA
 * - u-blox: `UBX-NAV-DOP` (`tDOP` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *tdop;

/**
 * @brief Vertical dilution of precision.
 *
 * How to get (examples):
 * - Android: NMEA `$GPGSA` (field 18)
 * - u-blox: `UBX-NAV-DOP` (`vDOP` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *vdop;

/**
 * @brief Horizontal dilution of precision.
 *
 * How to get (examples):
 * - Android: NMEA `$GPGSA` (field 16)
 * - u-blox: UBX-NAV-DOP` (`hDOP` field)
 *
 * - Note: Can be calculated if `ndop` and `edop` are known.
 */
@property (nonatomic, readonly, nullable) NSNumber *hdop;

/**
 * @brief Northing dilution of precision.
 *
 * How to get (examples):
 * - Android: Not available in NMEA.
 * - u-blox: `UBX-NAV-DOP` (`nDOP` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *ndop;

/**
 * @brief Easting dilution of precision.
 *
 * How to get (examples):
 * - Android: Not available in NMEA.
 * - u-blox: `UBX-NAV-DOP` (`eDOP` field)
 */
@property (nonatomic, readonly, nullable) NSNumber *edop;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief Raw GNSS satellites data for this epoch.
 *
 * How to get (examples):
 * - Android: From `GnssStatus` -> iterate from `0` to `GnssStatus.getSatelliteCount() - 1`
 * - u-blox: From `UBX-NAV-SAT` -> iterate through the repeating block in `UBX-NAV-SAT`
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRawGnssSatelliteData *> *satellites;

/**
 * @brief Monotonic timestamp for this set of measurements.
 *
 * How to get (examples):
 * - Android: Driver must capture `SystemClock.elapsedRealtimeNanos()`
 * - u-blox: Host-side timestamp captured by the driver upon message reception.
 */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToRawGnssData:(nonnull MBNNRawGnssData *)other;

@end
