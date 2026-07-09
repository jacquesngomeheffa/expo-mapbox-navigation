// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
@class MBXCoordinate2D;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Lane-level localization data of the ego vehicle.
 */
NS_SWIFT_NAME(LocalizedLaneData)
__attribute__((visibility ("default")))
@interface MBNNLocalizedLaneData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithCurrentLaneIndex:(nullable NSNumber *)currentLaneIndex
                                       laneCount:(nullable NSNumber *)laneCount
                               currentLaneOffset:(nullable NSNumber *)currentLaneOffset
                                currentLaneWidth:(nullable NSNumber *)currentLaneWidth
                               matchedCoordinate:(nullable MBXCoordinate2D *)matchedCoordinate
                                  matchedHeading:(nullable NSNumber *)matchedHeading
                   monotonicTimestampNanoseconds:(int64_t)monotonicTimestampNanoseconds;

/** Index of the lane that vehicle is on (from left to right). The index is zero based. */
@property (nonatomic, readonly, nullable) NSNumber *currentLaneIndex;

/** Number of the drivable lanes on the road (i.e: no shoulders and rescue lanes). */
@property (nonatomic, readonly, nullable) NSNumber *laneCount;

/**
 * Lane offset, in percentage of lane width. Negative means right of lane center,
 * positive means left of lane center (ISO 8855). Valid range - [-1, 1]
 */
@property (nonatomic, readonly, nullable) NSNumber *currentLaneOffset;

/** Lane width in meters */
@property (nonatomic, readonly, nullable) NSNumber *currentLaneWidth;

/** The coordinate of the ego vehicle matched on the HD map. */
@property (nonatomic, readonly, nullable) MBXCoordinate2D *matchedCoordinate;

/** The heading of the ego vehicle matched on the HD map. */
@property (nonatomic, readonly, nullable) NSNumber *matchedHeading;

/** Time when the data was created. The timestamp must be a monotonic timestamp. */
@property (nonatomic, readonly) int64_t monotonicTimestampNanoseconds;


- (BOOL)isEqualToLocalizedLaneData:(nonnull MBNNLocalizedLaneData *)other;

@end
