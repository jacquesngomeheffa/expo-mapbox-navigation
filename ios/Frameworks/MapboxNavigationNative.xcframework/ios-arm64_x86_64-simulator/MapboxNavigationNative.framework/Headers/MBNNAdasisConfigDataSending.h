// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNMessageBinaryFormat);

/**
 * Data sending configuration
 * messageIntervalMs - interval between sending messages in milliseconds
 * messagesInPackage - number of messages in one package (one message is 8 bytes)
 * metadataCycleSeconds - time in seconds between repetition of META-DATA message
 * enableRetransmission - if true, retransmission will be enabled (package will be amended with messages from previous cycles)
 * retransmissionMeters - after passing this distance, messages will not be retransmitted
 * treeTrailingLength - the trailing length of the path tree, relatively to the map-matched position, in the adasis provider
 * updatesEnabled - send messages with update flag
 * createPathOnOffsetOverflow - use the same path on offset overflow instead of creating a new path
 * setRouteIsReset - reset EHP on route change instead of triggering the update workflow
 */
NS_SWIFT_NAME(AdasisConfigDataSending)
__attribute__((visibility ("default")))
@interface MBNNAdasisConfigDataSending : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithOutputBinaryFormat:(MBNNMessageBinaryFormat)outputBinaryFormat;

- (nonnull instancetype)initWithOutputBinaryFormat:(MBNNMessageBinaryFormat)outputBinaryFormat
                                 messageIntervalMs:(uint32_t)messageIntervalMs
                                 messagesInPackage:(uint32_t)messagesInPackage
                              metadataCycleSeconds:(uint32_t)metadataCycleSeconds
                              enableRetransmission:(BOOL)enableRetransmission
                              retransmissionMeters:(uint32_t)retransmissionMeters
                                treeTrailingLength:(uint32_t)treeTrailingLength
                                    updatesEnabled:(BOOL)updatesEnabled
                      continuePathOnOffsetOverflow:(BOOL)continuePathOnOffsetOverflow
                                   setRouteIsReset:(BOOL)setRouteIsReset;

@property (nonatomic, readonly) MBNNMessageBinaryFormat outputBinaryFormat;
@property (nonatomic, readonly) uint32_t messageIntervalMs;
@property (nonatomic, readonly) uint32_t messagesInPackage;
@property (nonatomic, readonly) uint32_t metadataCycleSeconds;
@property (nonatomic, readonly) BOOL enableRetransmission;
@property (nonatomic, readonly) uint32_t retransmissionMeters;
@property (nonatomic, readonly) uint32_t treeTrailingLength;
@property (nonatomic, readonly) BOOL updatesEnabled;
@property (nonatomic, readonly) BOOL continuePathOnOffsetOverflow __attribute__((deprecated));
@property (nonatomic, readonly) BOOL setRouteIsReset;

- (BOOL)isEqualToAdasisConfigDataSending:(nonnull MBNNAdasisConfigDataSending *)other;

@end
