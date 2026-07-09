// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNLocationReferencePoint;
typedef NS_ENUM(NSInteger, MBNNOrientation);
typedef NS_ENUM(NSInteger, MBNNSideOfRoad);

NS_SWIFT_NAME(PointAlongLineLocationReference)
__attribute__((visibility ("default")))
@interface MBNNPointAlongLineLocationReference : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithPoints:(nonnull NSArray<MBNNLocationReferencePoint *> *)points
                        positiveOffset:(double)positiveOffset
                           orientation:(MBNNOrientation)orientation
                            sideOfRoad:(MBNNSideOfRoad)sideOfRoad;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Contains at least two points
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNLocationReferencePoint *> *points;

/** Represents as fraction of length since the beginning, lies in [0, 1) */
@property (nonatomic, readonly) double positiveOffset;

@property (nonatomic, readonly) MBNNOrientation orientation;
@property (nonatomic, readonly) MBNNSideOfRoad sideOfRoad;

@end
