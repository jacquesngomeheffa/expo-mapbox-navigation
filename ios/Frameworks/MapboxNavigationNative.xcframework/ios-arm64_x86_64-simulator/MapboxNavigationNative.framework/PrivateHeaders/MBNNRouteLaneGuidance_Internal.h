// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNConnectedLaneSequence;
@class MBNNRouteLaneGroup;

NS_SWIFT_NAME(RouteLaneGuidance)
__attribute__((visibility ("default")))
@interface MBNNRouteLaneGuidance : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithRouteId:(nonnull NSString *)routeId
                        routeLaneGroups:(nonnull NSArray<MBNNRouteLaneGroup *> *)routeLaneGroups
                 connectedLaneSequences:(nonnull NSArray<MBNNConnectedLaneSequence *> *)connectedLaneSequences;

/** Unique route id. Could be used to get smoothed edge geometries from `LaneGraphAccessor` */
@property (nonatomic, readonly, nonnull, copy) NSString *routeId;

@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteLaneGroup *> *routeLaneGroups;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNConnectedLaneSequence *> *connectedLaneSequences;

- (BOOL)isEqualToRouteLaneGuidance:(nonnull MBNNRouteLaneGuidance *)other;

@end
