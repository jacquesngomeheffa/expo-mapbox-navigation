// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Live-traffic closure along the route.
 */
NS_SWIFT_NAME(RdLegClosure)
__attribute__((visibility ("default")))
@interface MBNNRDTRdLegClosure : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithGeometryIndexStart:(uint32_t)geometryIndexStart
                                  geometryIndexEnd:(uint32_t)geometryIndexEnd;

/** The position in the coordinate list where the closure began, relative to the start of the leg it's on. */
@property (nonatomic, readonly) uint32_t geometryIndexStart;

/** The position in the coordinate list where the closure ended, relative to the start of the leg it's on. */
@property (nonatomic, readonly) uint32_t geometryIndexEnd;


@end
