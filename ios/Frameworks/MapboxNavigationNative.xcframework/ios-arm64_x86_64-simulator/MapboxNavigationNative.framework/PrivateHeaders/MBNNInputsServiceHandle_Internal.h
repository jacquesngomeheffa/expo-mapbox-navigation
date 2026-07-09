// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNAltimeterData;
@class MBNNCompassData;
@class MBNNConfigHandle;
@class MBNNDetectedObjects;
@class MBNNETCGateInfo;
@class MBNNFixLocation;
@class MBNNHistoryRecorderHandle;
@class MBNNImuTemperatureData;
@class MBNNLaneChangeAssistData;
@class MBNNLaneSensorInfo;
@class MBNNLocalizedLaneData;
@class MBNNOdometryData;
@class MBNNOrientationData;
@class MBNNPerceptionData;
@class MBNNRawAccelerometerData;
@class MBNNRawGnssData;
@class MBNNRawGravityData;
@class MBNNRawGyroscopeData;
@class MBNNSpeedData;
@class MBNNWeatherData;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Inputs service interface
 */
NS_SWIFT_NAME(InputsServiceHandle)
__attribute__((visibility ("default")))
@interface MBNNInputsServiceHandle : NSObject

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Constructs InputsService object with given dependencies
 *
 * @param config           config handle created with `ConfigFactory`
 * @param historyRecorder  history recorder created with `HistoryRecorderHandle.build` method
 */
+ (nonnull MBNNInputsServiceHandle *)buildForConfig:(nonnull MBNNConfigHandle *)config
                                    historyRecorder:(nullable MBNNHistoryRecorderHandle *)historyRecorder __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Data update
 */
- (void)updateOdometryDataForData:(nonnull MBNNOdometryData *)data;
- (void)updateRawLocationForLocation:(nonnull MBNNFixLocation *)location;
- (void)updateRawGnssDataForData:(nonnull MBNNRawGnssData *)data;
- (void)updateCompassDataForData:(nonnull MBNNCompassData *)data;
- (void)updateAltimeterDataForData:(nonnull MBNNAltimeterData *)data;
- (void)updateEtcGateInfoForInfo:(nonnull MBNNETCGateInfo *)info;
- (void)updateSpeedDataForData:(nonnull MBNNSpeedData *)data;
- (void)updateOrientationDataForData:(nonnull MBNNOrientationData *)data;
- (void)updateRawGyroscopeDataForData:(nonnull MBNNRawGyroscopeData *)data;
- (void)updateRawAccelerometerDataForData:(nonnull MBNNRawAccelerometerData *)data;
- (void)updateRawGravityDataForData:(nonnull MBNNRawGravityData *)data;
- (void)updateImuTemperatureDataForData:(nonnull MBNNImuTemperatureData *)data;
- (void)updateLaneSensorInfoForInfo:(nonnull MBNNLaneSensorInfo *)info;
- (void)updateWeatherDataForData:(nonnull MBNNWeatherData *)data;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Updates the current list of detected objects.
 * Note:
 * * the list of objects must contain unique object ids.
 * * the type of the object can change from one update to the next.
 *
 * @param objects The current list of detected objects
 */
- (void)updateDetectedObjectsForObjects:(nonnull MBNNDetectedObjects *)objects;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Updates the lane change assist data.
 *
 * @param data Data of a lane change assist functionality.
 */
- (void)updateLaneChangeAssistDataForData:(nonnull MBNNLaneChangeAssistData *)data;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Update lane-level localization data of the ego vehicle
 * @param laneData Localization data to position ego vehicle precisely
 */
- (void)updateLocalizedLaneDataForLaneData:(nonnull MBNNLocalizedLaneData *)laneData;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Update perception data that is provided by the perception system.
 * @param perceptionData The perception data to update.
 */
- (void)updatePerceptionDataForPerceptionData:(nonnull MBNNPerceptionData *)perceptionData;

@end
