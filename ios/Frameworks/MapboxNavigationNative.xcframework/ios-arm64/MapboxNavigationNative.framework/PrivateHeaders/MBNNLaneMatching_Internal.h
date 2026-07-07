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

@property (nonatomic, readonly, nonnull) MBNNFixDirectedCoordinate *smoothedCoordinate;
@property (nonatomic, readonly, nullable) MBNNLanePosition *smoothedLanePosition;
@property (nonatomic, readonly, nonnull) MBNNFixDirectedCoordinate *snappedCoordinate;
@property (nonatomic, readonly, nonnull) MBNNLanePosition *lanePosition;
@property (nonatomic, readonly, nonnull) MBNNMatchedLaneInfo *laneInfo;
@property (nonatomic, readonly, nullable, copy) NSArray<MBNNMatchedDetectedObject *> *matchedDetectedObjects;
/**
 * Boolean flag defining if lane change has just occurred,
 * i.e. snapped location have just jumped onto left or right lane.
 */
@property (nonatomic, readonly) BOOL isLaneChange;


- (BOOL)isEqualToLaneMatching:(nonnull MBNNLaneMatching *)other;

@end
