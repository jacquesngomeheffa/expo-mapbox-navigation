// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * RoadObjectsStore observer.
 */
NS_SWIFT_NAME(RoadObjectsStoreObserver)
@protocol MBNNRoadObjectsStoreObserver
/**
 * Notifies that new road object was added to the store
 *
 * @param id  road object id
 */
- (void)onRoadObjectAddedForId:(nonnull NSString *)id_;
/**
 * Notifies that road object with given id was updated
 *
 * @param id  road object id
 */
- (void)onRoadObjectUpdatedForId:(nonnull NSString *)id_;
/**
 * Notifies that road object with given id was removed from the store
 *
 * @param id  road object id
 */
- (void)onRoadObjectRemovedForId:(nonnull NSString *)id_;
/**
 * Notifies that custom road object with given id was matched
 *
 * @param id  road object id
 */
- (void)onCustomRoadObjectMatchedForId:(nonnull NSString *)id_;
/**
 * Notifies that custom road object adding with given id was cancelled
 *
 * @param id  road object id
 */
- (void)onCustomRoadObjectAddingCancelledForId:(nonnull NSString *)id_;
/**
 * Notifies that matching of custom road object with given id failed
 *
 * @param id  road object id
 */
- (void)onCustomRoadObjectMatchingFailedForId:(nonnull NSString *)id_;
@end
