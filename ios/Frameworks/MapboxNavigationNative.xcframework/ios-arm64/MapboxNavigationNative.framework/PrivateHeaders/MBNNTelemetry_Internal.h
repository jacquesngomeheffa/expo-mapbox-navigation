// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNOuterDeviceAction_Internal.h>
#import <MapboxNavigationNative/MBNNUserFeedbackCallback_Internal.h>

@class MBNNUserFeedback;
@class MBNNUserFeedbackHandle;
@class MBNNUserFeedbackMetadata;

NS_SWIFT_NAME(Telemetry)
@protocol MBNNTelemetry
/**
 * Send custom telemetry event. The custom event is intended to be used by platform sdks to test hypotheses,
 * send a temporary events. So type is not specified, it's up to the platforms needs.
 *
 * @param type     type of custom event
 * @param version  version of the custom event
 * @param payload  payload of custom event, in JSON format
 */
- (void)postTelemetryCustomEventForType:(nonnull NSString *)type
                                version:(nonnull NSString *)version
                                payload:(nullable NSString *)payload;
- (void)postOuterDeviceEventForAction:(MBNNOuterDeviceAction)action;
- (nonnull MBNNUserFeedbackHandle *)startBuildUserFeedbackMetadata;
- (void)postUserFeedbackForFeedbackMetadata:(nonnull MBNNUserFeedbackMetadata *)feedbackMetadata
                               userFeedback:(nonnull MBNNUserFeedback *)userFeedback
                                   callback:(nonnull MBNNUserFeedbackCallback)callback;
@end
