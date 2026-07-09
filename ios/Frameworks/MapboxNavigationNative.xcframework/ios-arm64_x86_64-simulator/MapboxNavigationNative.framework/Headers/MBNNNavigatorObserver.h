// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNNavigationStatus;
@class MBNNRoutesChangeInfo;
typedef NS_ENUM(NSInteger, MBNNNavigationStatusOrigin);

/**
 * Notifications from Navigator.
 *
 * All methods are invoked on `common::Scheduler` of thread on which Navigator has been constructed.
 */
NS_SWIFT_NAME(NavigatorObserver)
@protocol MBNNNavigatorObserver
/**
 * Invoked when a new navigation status is calculated.
 *
 * There are several cases when it can be called:
 * 1. After receiving new raw location via `updateLocation`
 * 2. After transition to active guidance via `setRoute/changeRouteLeg` methods (at least one raw location should be received before)
 * 3. Unconditionally if we have no raw locations for some significant amount of time
 *    (controlled via `unconditionalStatusPollingPatienceSeconds` and `unconditionalStatusPollingIntervalSeconds` fields in config)
 */
- (void)onStatusForOrigin:(MBNNNavigationStatusOrigin)origin
                   status:(nonnull MBNNNavigationStatus *)status;
/**
 * Invoked when a new primary route and alternative routes have been set by the Navigator.
 * Only alternative routes change may happen without primary change.
 */
- (void)onRoutesChangedForInfo:(nonnull MBNNRoutesChangeInfo *)info;
@end
