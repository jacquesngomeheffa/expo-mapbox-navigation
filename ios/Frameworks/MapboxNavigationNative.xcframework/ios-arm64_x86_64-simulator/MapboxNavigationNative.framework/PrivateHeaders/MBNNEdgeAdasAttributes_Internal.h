// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNSpeedLimitInfo;
@class MBNNValueOnEdge;

NS_SWIFT_NAME(EdgeAdasAttributes)
__attribute__((visibility ("default")))
@interface MBNNEdgeAdasAttributes : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithSpeedLimit:(nonnull NSArray<MBNNSpeedLimitInfo *> *)speedLimit
                                    slopes:(nonnull NSArray<MBNNValueOnEdge *> *)slopes
                                curvatures:(nonnull NSArray<MBNNValueOnEdge *> *)curvatures
                             isDividedRoad:(nullable NSNumber *)isDividedRoad;

@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNSpeedLimitInfo *> *speedLimit;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNValueOnEdge *> *slopes;
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNValueOnEdge *> *curvatures;
/** A flag indicating if the edge is a divided road. */
@property (nonatomic, readonly, nullable) NSNumber *isDividedRoad;


- (BOOL)isEqualToEdgeAdasAttributes:(nonnull MBNNEdgeAdasAttributes *)other;

@end
