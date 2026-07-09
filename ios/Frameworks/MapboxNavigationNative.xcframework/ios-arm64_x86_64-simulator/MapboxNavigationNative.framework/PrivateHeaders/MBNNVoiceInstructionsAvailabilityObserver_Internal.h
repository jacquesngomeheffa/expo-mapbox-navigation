// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Observer for playing last voice instruction availability changes in the UI
 */
NS_SWIFT_NAME(VoiceInstructionsAvailabilityObserver)
@protocol MBNNVoiceInstructionsAvailabilityObserver
/** called from the main thread when voice instruction availability changes */
- (void)onVoiceInstructionAvailabilityChangedForAvailable:(BOOL)available;
@end
