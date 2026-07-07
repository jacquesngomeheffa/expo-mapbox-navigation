// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNGenerateEh360Callback_Internal.h>

@class MBNNETCGateInfo;

NS_SWIFT_NAME(Experimental)
@protocol MBNNExperimental
- (void)generateEh360ForGraphTraversalRadiusInMeters:(double)graphTraversalRadiusInMeters
                                            callback:(nonnull MBNNGenerateEh360Callback)callback __attribute__((deprecated));
- (void)updateETCGateInfoForEtcGateInfo:(nonnull MBNNETCGateInfo *)etcGateInfo __attribute__((deprecated));
@end
