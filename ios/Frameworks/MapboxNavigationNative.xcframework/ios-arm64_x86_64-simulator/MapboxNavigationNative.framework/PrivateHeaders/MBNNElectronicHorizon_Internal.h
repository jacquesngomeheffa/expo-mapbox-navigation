// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNElectronicHorizonEdge;

NS_SWIFT_NAME(ElectronicHorizon)
__attribute__((visibility ("default")))
@interface MBNNElectronicHorizon : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithStart:(nonnull MBNNElectronicHorizonEdge *)start;

@property (nonatomic, readonly, nonnull) MBNNElectronicHorizonEdge *start;

@end
