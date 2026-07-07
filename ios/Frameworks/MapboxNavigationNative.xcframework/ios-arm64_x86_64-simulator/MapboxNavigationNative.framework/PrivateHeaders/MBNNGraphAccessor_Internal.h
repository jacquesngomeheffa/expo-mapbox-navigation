// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
@class MBXCoordinate2D;

@class MBNNCacheHandle;
@class MBNNEdgeAdasAttributes;
@class MBNNEdgeMetadata;
@class MBNNGraphPath;
@class MBNNGraphPosition;

NS_SWIFT_NAME(GraphAccessor)
__attribute__((visibility ("default")))
@interface MBNNGraphAccessor : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithCache:(nonnull MBNNCacheHandle *)cache;

- (nullable MBNNEdgeMetadata *)getEdgeMetadataForEdgeId:(uint64_t)edgeId __attribute((ns_returns_retained));
/**
 * Returns Graph Edge geometry for the given GraphId of the edge.
 * If edge with given edgeId is not accessible, returns null
 */
- (nullable NSArray<MBXCoordinate2D *> *)getEdgeShapeForEdgeId:(uint64_t)edgeId __attribute((ns_returns_retained));
- (nullable NSArray<MBXCoordinate2D *> *)getPathShapeForPath:(nonnull MBNNGraphPath *)path __attribute((ns_returns_retained));
- (nullable MBXCoordinate2D *)getPositionCoordinateForPosition:(nonnull MBNNGraphPosition *)position __attribute((ns_returns_retained));
- (nullable MBNNEdgeAdasAttributes *)getAdasAttributesForEdgeId:(uint64_t)edgeId __attribute((ns_returns_retained));

@end
