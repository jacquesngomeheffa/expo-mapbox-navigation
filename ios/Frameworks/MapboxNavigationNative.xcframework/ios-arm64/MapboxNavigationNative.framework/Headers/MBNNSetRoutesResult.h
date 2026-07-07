// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouteAlternative;
@protocol MBNNRouteInterface;

NS_SWIFT_NAME(SetRoutesResult)
__attribute__((visibility ("default")))
@interface MBNNSetRoutesResult : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;


/** Primary route set to navigator. */
@property (nonatomic, readonly, nullable) id<MBNNRouteInterface> primaryRoute;


@end
