// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNGate;

NS_SWIFT_NAME(PolygonDistanceInfo)
__attribute__((visibility ("default")))
@interface MBNNPolygonDistanceInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithInside:(BOOL)inside
                             entrances:(nonnull NSArray<MBNNGate *> *)entrances
                                 exits:(nonnull NSArray<MBNNGate *> *)exits;

/** `true` if position is inside polygon, `false` otherwise */
@property (nonatomic, readonly) BOOL inside;

@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNGate *> *entrances;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNGate *> *exits;

@end
