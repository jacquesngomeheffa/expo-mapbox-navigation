// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNEventsMetadata;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Clients must provide an implementation of EventsMetadataInterface to pass required metadata to
 * the telemetry subsystem. Implementation must be thread-safe.
 */
NS_SWIFT_NAME(EventsMetadataInterface)
@protocol MBNNEventsMetadataInterface
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Pass EventsMetadata for filling the events properties
 */
- (nonnull MBNNEventsMetadata *)provideEventsMetadata;
@end
