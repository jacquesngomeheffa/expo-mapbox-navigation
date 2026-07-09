// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Voice instruction.
 */
NS_SWIFT_NAME(RdVoiceInstruction)
__attribute__((visibility ("default")))
@interface MBNNRDTRdVoiceInstruction : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithDistanceAlongGeometry:(float)distanceAlongGeometry
                                         announcement:(nonnull NSString *)announcement
                                     ssmlAnnouncement:(nonnull NSString *)ssmlAnnouncement;

/** Distance from the upcoming maneuver the voice instruction should begin, in meters. */
@property (nonatomic, readonly) float distanceAlongGeometry;

/** The text of the verbal instruction. */
@property (nonatomic, readonly, nonnull, copy) NSString *announcement;

/** SSML markup for proper text and pronunciation. */
@property (nonatomic, readonly, nonnull, copy) NSString *ssmlAnnouncement;


@end
