// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNTurnSignalStatus_Internal.h>

NS_SWIFT_NAME(TurnSignalStatusData)
__attribute__((visibility ("default")))
@interface MBNNTurnSignalStatusData : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithTurnSignalStatus:(MBNNTurnSignalStatus)turnSignalStatus
                                       timestamp:(nonnull NSDate *)timestamp;

@property (nonatomic, readonly) MBNNTurnSignalStatus turnSignalStatus;
/** Time when the data was created */
@property (nonatomic, readonly, nonnull) NSDate *timestamp;


@end
