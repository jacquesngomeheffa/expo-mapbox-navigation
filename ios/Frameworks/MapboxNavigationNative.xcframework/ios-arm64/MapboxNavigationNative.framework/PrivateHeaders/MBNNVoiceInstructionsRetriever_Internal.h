// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNVoiceInstructionsCallback_Internal.h>

@class MBNNVoiceInstruction;
@protocol MBNNVoiceInstructionsAvailabilityObserver;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Retriever for voice instructions
 *
 * This API is experimental and may be changed eventually
 * Use Navigator the retrieve the instance of VoiceInstructionsRetriever and subscribe to availability changes
 */
NS_SWIFT_NAME(VoiceInstructionsRetriever)
@protocol MBNNVoiceInstructionsRetriever
/**
 * Returns Empty array if no available instruction or no route at all
 * Returns a set of relevant instructions on the current step till their expiration
 * Returns next instruction (potentially from next step), if all played instructions become irrelevant (passed).
 * Voice instruction template is applied to the current state as of the time of the call, e.g. all distances are instantiated using the most recent navigation status.
 */
- (nonnull NSArray<MBNNVoiceInstruction *> *)getRelevantVoiceInstructions __attribute__((deprecated));
- (void)getRelevantVoiceInstructionsForCallback:(nonnull MBNNVoiceInstructionsCallback)callback;
/**
 *  Current state of presence of last instructions.
 *  Please use VoiceInstructionsAvailabilityObserver for notification on the state.
 *  This method is for initialization purposes only.
 */
- (BOOL)isInstructionAvailable;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Subscribes to voice instruction availability changes.
 * The observer will be called from the main thread when voice instruction availability changes.
 */
- (void)subscribeForObserver:(nonnull id<MBNNVoiceInstructionsAvailabilityObserver>)observer;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Unsubscribes from voice instruction availability changes.
 * The observer will no longer be called when voice instruction availability changes.
 */
- (void)unsubscribeForObserver:(nonnull id<MBNNVoiceInstructionsAvailabilityObserver>)observer;
@end
