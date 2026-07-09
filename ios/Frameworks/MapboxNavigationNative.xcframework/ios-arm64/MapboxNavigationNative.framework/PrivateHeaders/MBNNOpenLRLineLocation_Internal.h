// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXGeometry;

@class MBNNGraphPath;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *  Defines a location of a line on the road graph
 */
NS_SWIFT_NAME(OpenLRLineLocation)
__attribute__((visibility ("default")))
@interface MBNNOpenLRLineLocation : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Position of a line on a road graph
 */
- (nonnull MBNNGraphPath *)getPath __attribute((ns_returns_retained));
/** Shape of a line */
- (nonnull MBXGeometry *)getShape __attribute((ns_returns_retained));

@end
