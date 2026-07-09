// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * State information wrapper of a detected object
 */
NS_SWIFT_NAME(DetectionState)
__attribute__((visibility ("default")))
@interface MBNNDetectionState : NSObject

- (nonnull instancetype)init;

- (nonnull instancetype)initWithLeftBlinkerOn:(BOOL)leftBlinkerOn
                               rightBlinkerOn:(BOOL)rightBlinkerOn
                                      brakeOn:(BOOL)brakeOn
                                      warning:(BOOL)warning
                                    highlight:(BOOL)highlight;

@property (nonatomic, readonly) BOOL leftBlinkerOn;
@property (nonatomic, readonly) BOOL rightBlinkerOn;
@property (nonatomic, readonly) BOOL brakeOn;
@property (nonatomic, readonly) BOOL warning;
@property (nonatomic, readonly) BOOL highlight;

- (BOOL)isEqualToDetectionState:(nonnull MBNNDetectionState *)other;

@end
