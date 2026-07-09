// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNForceRerouteCallback_Internal.h>

typedef NS_ENUM(NSInteger, MBNNForceRerouteReason);

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * A detector interface to initiate reroute events
 */
NS_SWIFT_NAME(RerouteDetectorInterface)
__attribute__((deprecated))
@protocol MBNNRerouteDetectorInterface
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *  Force reroute with the reason provided. Reroute result is notified to observers.
 */
- (void)forceRerouteForReason:(MBNNForceRerouteReason)reason;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *  Force reroute with the reason provided. Reroute result is provided to a callback **and NOT notified to observers**.
 */
- (void)forceRerouteForReason:(MBNNForceRerouteReason)reason
                     callback:(nonnull MBNNForceRerouteCallback)callback;
/**
 *  Returns true in between reroute request started and observers or a callback notified. Doesn't include
 *  user's reaction to a reroute (expected setRoute(...))
 */
- (BOOL)isReroute;
/** Cancels reroute if in progress, has no effect otherwise */
- (void)cancelReroute;
@end
