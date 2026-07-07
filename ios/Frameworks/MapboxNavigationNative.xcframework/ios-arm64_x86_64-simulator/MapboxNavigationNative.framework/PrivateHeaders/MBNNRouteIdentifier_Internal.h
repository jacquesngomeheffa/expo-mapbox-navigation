// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

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
- (nonnull NSString *)getRouteId __attribute((ns_returns_retained));

/** The UUID of the route */
@property (nonatomic, readonly, nonnull, copy) NSString *uuid;

/** The index of the route */
@property (nonatomic, readonly) uint32_t index;


- (BOOL)isEqualToRouteIdentifier:(nonnull MBNNRouteIdentifier *)other;

@end
