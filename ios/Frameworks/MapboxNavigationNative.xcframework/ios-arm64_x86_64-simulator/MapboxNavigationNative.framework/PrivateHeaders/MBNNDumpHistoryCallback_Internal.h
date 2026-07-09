// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Callback which called as a result of `stopRecording`
 *
 * @param `filepath` could be null if `stopRecording` called without any actually received events
 * or if `HistoryRecorder` destroyed before `stopRecording` ends it's work (as an asynchronous method).
 */
NS_SWIFT_NAME(DumpHistoryCallback)
typedef void (^MBNNDumpHistoryCallback)(NSString * _Nullable filepath); // NOLINT(modernize-use-using)
