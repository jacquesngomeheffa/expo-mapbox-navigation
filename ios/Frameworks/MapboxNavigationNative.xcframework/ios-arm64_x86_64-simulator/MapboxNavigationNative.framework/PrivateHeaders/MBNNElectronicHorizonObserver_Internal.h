// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNElectronicHorizonPosition;
@class MBNNRoadObjectDistance;
@class MBNNRoadObjectEnterExitInfo;
@class MBNNRoadObjectPassInfo;

NS_SWIFT_NAME(ElectronicHorizonObserver)
@protocol MBNNElectronicHorizonObserver
- (void)onPositionUpdatedForPosition:(nonnull MBNNElectronicHorizonPosition *)position
                           distances:(nonnull NSArray<MBNNRoadObjectDistance *> *)distances;
- (void)onRoadObjectEnterForInfo:(nonnull MBNNRoadObjectEnterExitInfo *)info;
- (void)onRoadObjectExitForInfo:(nonnull MBNNRoadObjectEnterExitInfo *)info;
- (void)onRoadObjectPassedForInfo:(nonnull MBNNRoadObjectPassInfo *)info;
@end
