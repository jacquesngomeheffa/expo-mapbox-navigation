// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNFixDirectedCoordinate;
@class MBNNLanePosition;
@class MBNNMatchedDetectedObject;
@class MBNNMatchedLaneInfo;

NS_SWIFT_NAME(LaneMatching)
__attribute__((visibility ("default")))
@interface MBNNLaneMatching : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithSmoothedCoordinate:(nonnull MBNNFixDirectedCoordinate *)smoothedCoordinate
                              smoothedLanePosition:(nullable MBNNLanePosition *)smoothedLanePosition
                                 snappedCoordinate:(nonnull MBNNFixDirectedCoordinate *)snappedCoordinate
                                      lanePosition:(nonnull MBNNLanePosition *)lanePosition
                                          laneInfo:(nonnull MBNNMatchedLaneInfo *)laneInfo
                            matchedDetectedObjects:(nullable NSArray<MBNNMatchedDetectedObject *> *)matchedDetectedObjects
                                      isLaneChange:(BOOL)isLaneChange;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Smoothed coordinate of the vehicle, supposed to be shown to a customer.
 * This coordinate is generated based on the snapped coordinate and optionally on the raw GPS coordinate.
 * The main purpose of it is to smooth movement of the pack during some maneuvers,
 * e.g. smooth lane change or teleports (jumps from one road to another).
 * For more details: https://mapbox.atlassian.net/wiki/spaces/3ddrive/pages/710902130/Lane+Coordinate+Smoother
 */
@property (nonatomic, readonly, nonnull) MBNNFixDirectedCoordinate *smoothedCoordinate;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Smoothed lane position. In most cases would be the same as snapped `lanePosition`.
 * If lane change smoothing is in progress - that lane position would correspond to the nearest lane.
 * If teleport smoothing is in progress - that lane position would be empty.
 */
@property (nonatomic, readonly, nullable) MBNNLanePosition *smoothedLanePosition;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Coordinate of the vehicle snapped on the map-matched lane
 */
@property (nonatomic, readonly, nonnull) MBNNFixDirectedCoordinate *snappedCoordinate;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Snapped lane position
 */
@property (nonatomic, readonly, nonnull) MBNNLanePosition *lanePosition;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Lane information of the vehicle
 */
@property (nonatomic, readonly, nonnull) MBNNMatchedLaneInfo *laneInfo;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Lane information of the detected objects
 */
@property (nonatomic, readonly, nullable, copy) NSArray<MBNNMatchedDetectedObject *> *matchedDetectedObjects;

/**
 * Boolean flag defining if lane change has just occurred,
 * i.e. snapped location have just jumped onto left or right lane.
 */
@property (nonatomic, readonly) BOOL isLaneChange;


- (BOOL)isEqualToLaneMatching:(nonnull MBNNLaneMatching *)other;

@end
