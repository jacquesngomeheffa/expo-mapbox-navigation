// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
@class MBXExpected<__covariant Value, __covariant Error>;

@class MBNNRoadObject;
@class MBNNRoadObjectMatcherError;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * The listener should be set to RoadObjectMatcher before to start working with
 * it.
 */
NS_SWIFT_NAME(RoadObjectMatcherListener)
@protocol MBNNRoadObjectMatcherListener
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Called if a RoadObject was matched or an error happened.
 */
- (void)onRoadObjectMatchedForRoadObject:(nonnull MBXExpected<MBNNRoadObject *, MBNNRoadObjectMatcherError *> *)roadObject;
/** Called if matching a RoadObject with the specified id was cancelled. */
- (void)onMatchingCancelledForId:(nonnull NSString *)id_;
@end
