// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXGeometry;

@class MBNNPosition;

NS_SWIFT_NAME(MatchedPolygonLocation)
__attribute__((visibility ("default")))
@interface MBNNMatchedPolygonLocation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Positions of polygon entries
 */
- (nonnull NSArray<MBNNPosition *> *)getEntries __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Positions of polygon exits
 */
- (nonnull NSArray<MBNNPosition *> *)getExits __attribute((ns_returns_retained));
/** Shape of a polygon */
- (nonnull MBXGeometry *)getShape __attribute((ns_returns_retained));

@end
