// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNFixDirectedCoordinate;

NS_SWIFT_NAME(Projection)
__attribute__((visibility ("default")))
@interface MBNNProjection : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(uint64_t)id_
                        coordinate:(nonnull MBNNFixDirectedCoordinate *)coordinate
                          distance:(double)distance
                      percentAlong:(double)percentAlong;

/** Feature id. */
@property (nonatomic, readonly) uint64_t id;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Coordinate on a feature's geometry. Coordinate's heading corresponds to segment's heading.
 */
@property (nonatomic, readonly, nonnull) MBNNFixDirectedCoordinate *coordinate;

/** Distance in meters between original search coordinate and projected coordinate. */
@property (nonatomic, readonly) double distance;

/** Percent along feature's geometry. 0.0 means start of the geometry, 1.0 - end of the geometry. */
@property (nonatomic, readonly) double percentAlong;


- (BOOL)isEqualToProjection:(nonnull MBNNProjection *)other;

@end
