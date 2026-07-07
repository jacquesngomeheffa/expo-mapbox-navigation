// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <MapboxNavigationNative/MBNNDumpHistoryCallback_Internal.h>

@class MBNNConfigHandle;
@class MBNNSdkHistoryInfo;

NS_SWIFT_NAME(HistoryRecorderHandle)
__attribute__((visibility ("default")))
@interface MBNNHistoryRecorderHandle : NSObject

/**
 * Starts history recording session.
 * If history recording is already started - does nothing.
 * The new history recording session will contain "history context" - some events which occurred before the `startRecording()` was called.
 * These "history context" events will be separated with a special `separator` event from the actual session events.
 * @return  A list of file paths in which the history recording session will be written
 */
- (nonnull NSArray<NSString *> *)startRecording __attribute((ns_returns_retained));
- (void)stopRecordingForResult:(nonnull MBNNDumpHistoryCallback)result;
/**
 * Adds a custom event to the navigators history. This can be useful to log things that
 * happen during navigation that are specific to your application.
 * @param  eventType  the event type in the events log for your custom even
 * @param  eventJson  the json to attach to the "properties" key of the event
 */
- (void)pushHistoryForEventType:(nonnull NSString *)eventType
                      eventJson:(nonnull NSString *)eventJson;
+ (nullable MBNNHistoryRecorderHandle *)buildForHistoryDir:(nonnull NSString *)historyDir
                                                   sdkInfo:(nonnull MBNNSdkHistoryInfo *)sdkInfo
                                                    config:(nonnull MBNNConfigHandle *)config __attribute((ns_returns_retained));
+ (nullable MBNNHistoryRecorderHandle *)buildCompositeRecorderForRecorders:(nonnull NSArray<MBNNHistoryRecorderHandle *> *)recorders __attribute((ns_returns_retained));

@end
