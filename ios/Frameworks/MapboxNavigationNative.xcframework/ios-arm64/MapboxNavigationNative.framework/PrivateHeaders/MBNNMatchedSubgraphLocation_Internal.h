// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXGeometry;

@class MBNNPosition;
@class MBNNSubgraphEdge;

NS_SWIFT_NAME(MatchedSubgraphLocation)
__attribute__((visibility ("default")))
@interface MBNNMatchedSubgraphLocation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull NSArray<MBNNPosition *> *)getEnters __attribute((ns_returns_retained));
- (nonnull NSArray<MBNNPosition *> *)getExits __attribute((ns_returns_retained));
- (nonnull NSDictionary<NSNumber *, MBNNSubgraphEdge *> *)getEdges __attribute((ns_returns_retained));
/** Geometry of the subgraph */
- (nonnull MBXGeometry *)getShape __attribute((ns_returns_retained));

@end
