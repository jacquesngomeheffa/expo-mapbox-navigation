// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNAssistedDrivingLaneChangeDirection_Internal.h>
#import <MapboxNavigationNative/MBNNAssistedDrivingState_Internal.h>

NS_SWIFT_NAME(AssistedDrivingData)
__attribute__((visibility ("default")))
@interface MBNNAssistedDrivingData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithState:(MBNNAssistedDrivingState)state
                            direction:(nullable NSNumber *)direction
                            timestamp:(nonnull NSDate *)timestamp;

@property (nonatomic, readonly) MBNNAssistedDrivingState state;
@property (nonatomic, readonly, nullable) NSNumber *direction;
/** Time when the data was created */
@property (nonatomic, readonly, nonnull) NSDate *timestamp;


@end
