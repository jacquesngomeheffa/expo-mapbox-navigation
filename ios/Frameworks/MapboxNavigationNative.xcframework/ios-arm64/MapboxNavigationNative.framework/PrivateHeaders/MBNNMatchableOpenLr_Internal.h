// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNOpenLR;

NS_SWIFT_NAME(MatchableOpenLr)
__attribute__((visibility ("default")))
@interface MBNNMatchableOpenLr : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithOpenlr:(nonnull MBNNOpenLR *)openlr
                                    id:(nonnull NSString *)id_;

@property (nonatomic, readonly, nonnull) MBNNOpenLR *openlr;
/** id of the resulting object */
@property (nonatomic, readonly, nonnull, copy) NSString *id;


@end
