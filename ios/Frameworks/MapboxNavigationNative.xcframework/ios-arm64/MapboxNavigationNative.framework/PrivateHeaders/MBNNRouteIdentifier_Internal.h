// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Identifies a specific route by its UUID and index
 */
NS_SWIFT_NAME(RouteIdentifier)
__attribute__((visibility ("default")))
@interface MBNNRouteIdentifier : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithUuid:(nonnull NSString *)uuid
                               index:(uint32_t)index;

/**
 * Unique route id.
 * Format: UUID + "#" + index
 * Example: d77PcddF8rhGUc3ORYGfcwcDfS_8QW6r1iXugXD0HOgmr9CWL8wn0g==#0
 */
- (nonnull NSString *)toRouteIdString __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Reverse of getRouteId
 */
+ (nonnull MBNNRouteIdentifier *)fromStringForRouteId:(nonnull NSString *)routeId __attribute((ns_returns_retained));

/** The UUID of the route */
@property (nonatomic, readonly, nonnull, copy) NSString *uuid;

/** The index of the route */
@property (nonatomic, readonly) uint32_t index;


- (BOOL)isEqualToRouteIdentifier:(nonnull MBNNRouteIdentifier *)other;

@end
