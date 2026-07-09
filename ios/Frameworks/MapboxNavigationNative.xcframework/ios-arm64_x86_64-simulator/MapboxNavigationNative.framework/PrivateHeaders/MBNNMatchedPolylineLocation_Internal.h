// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXGeometry;

@class MBNNGraphPath;
@class MBNNMatchedPolylinePart;

NS_SWIFT_NAME(MatchedPolylineLocation)
__attribute__((visibility ("default")))
@interface MBNNMatchedPolylineLocation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Position of a polyline on a road graph
 */
- (nonnull MBNNGraphPath *)getPath __attribute((ns_returns_retained));
/** Shape of a polyline */
- (nonnull MBXGeometry *)getShape __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Exists if the polyline is being matched partially. Contains information
 * about the already matched part.
 */
- (nullable MBNNMatchedPolylinePart *)getMatchedPart __attribute((ns_returns_retained));

@end
