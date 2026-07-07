// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNCacheHandle;
@class MBNNMatchableGeometry;
@class MBNNMatchableOpenLr;
@class MBNNMatchablePoint;
@class MBNNMatchingOptions;
@class MBNNRoadObjectMatcherConfig;
@protocol MBNNRoadObjectMatcherListener;

NS_SWIFT_NAME(RoadObjectMatcher)
__attribute__((visibility ("default")))
@interface MBNNRoadObjectMatcher : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithCache:(nonnull MBNNCacheHandle *)cache;

- (nonnull instancetype)initWithCache:(nonnull MBNNCacheHandle *)cache
                               config:(nonnull MBNNRoadObjectMatcherConfig *)config;

- (void)setListenerForListener:(nullable id<MBNNRoadObjectMatcherListener>)listener;
- (void)matchOpenLRsForOpenLrs:(nonnull NSArray<MBNNMatchableOpenLr *> *)openLrs
                       options:(nonnull MBNNMatchingOptions *)options;
- (void)matchPolylinesForPolylines:(nonnull NSArray<MBNNMatchableGeometry *> *)polylines
                           options:(nonnull MBNNMatchingOptions *)options;
- (void)matchPolygonsForPolygons:(nonnull NSArray<MBNNMatchableGeometry *> *)polygons
                         options:(nonnull MBNNMatchingOptions *)options;
- (void)matchGantriesForGantries:(nonnull NSArray<MBNNMatchableGeometry *> *)gantries
                         options:(nonnull MBNNMatchingOptions *)options;
- (void)matchPointsForPoints:(nonnull NSArray<MBNNMatchablePoint *> *)points
                     options:(nonnull MBNNMatchingOptions *)options;
/** Cancels a batch of previously scheduled objects matching. */
- (void)cancelForIds:(nonnull NSArray<NSString *> *)ids;
/** Cancels all scheduled matchings. */
- (void)cancelAll;

@end
