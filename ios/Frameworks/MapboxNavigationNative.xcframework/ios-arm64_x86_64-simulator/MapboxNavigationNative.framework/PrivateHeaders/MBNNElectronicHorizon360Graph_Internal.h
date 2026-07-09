// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNElectronicHorizon360Edge;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * ElectronicHorizon360 graph. Represents ElectronicHorizon360.
 */
NS_SWIFT_NAME(ElectronicHorizon360Graph)
__attribute__((visibility ("default")))
@interface MBNNElectronicHorizon360Graph : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithOrigin:(uint64_t)origin
                                 graph:(nonnull NSDictionary<NSNumber *, MBNNElectronicHorizon360Edge *> *)graph;

/** Start edge. */
@property (nonatomic, readonly) uint64_t origin;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Map where key is edge id, and value is `ElectronicHorizon360Edge`.
 * Traversing will look this: first we take origin edge, and look in map.
 * Then we take `branchLinks` from `ElectronicHorizon360Edge` and do the same steps for all outgoing Edges.
 */
@property (nonatomic, readonly, nonnull, copy) NSDictionary<NSNumber *, MBNNElectronicHorizon360Edge *> *graph;


@end
