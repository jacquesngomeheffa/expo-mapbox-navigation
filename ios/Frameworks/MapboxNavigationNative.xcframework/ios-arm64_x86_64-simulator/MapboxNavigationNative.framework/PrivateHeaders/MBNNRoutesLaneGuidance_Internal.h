// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteLaneGuidance;

NS_SWIFT_NAME(RoutesLaneGuidance)
__attribute__((visibility ("default")))
@interface MBNNRoutesLaneGuidance : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithCurrent:(nonnull MBNNRouteLaneGuidance *)current
                           alternatives:(nonnull NSArray<MBNNRouteLaneGuidance *> *)alternatives;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Route lane guidance for the current route.
 */
@property (nonatomic, readonly, nonnull) MBNNRouteLaneGuidance *current;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Route lane guidance for each alternative.
 *
 * The number of alternatives depends on the route request. The alternatives themselves may change
 * over time due to "Continuous Alternatives" feature.
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouteLaneGuidance *> *alternatives;


- (BOOL)isEqualToRoutesLaneGuidance:(nonnull MBNNRoutesLaneGuidance *)other;

@end
