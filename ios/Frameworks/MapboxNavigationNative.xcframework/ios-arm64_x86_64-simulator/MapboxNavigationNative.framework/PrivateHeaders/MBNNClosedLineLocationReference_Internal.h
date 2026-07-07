// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNLineAttributes;
@class MBNNLocationReferencePoint;

NS_SWIFT_NAME(ClosedLineLocationReference)
__attribute__((visibility ("default")))
@interface MBNNClosedLineLocationReference : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithPoints:(nonnull NSArray<MBNNLocationReferencePoint *> *)points
                              lastLine:(nonnull MBNNLineAttributes *)lastLine;

@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNLocationReferencePoint *> *points;
@property (nonatomic, readonly, nonnull) MBNNLineAttributes *lastLine;

@end
