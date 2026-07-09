// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNWaypoint.h>

@class MBNNTimeZone;
typedef NS_ENUM(NSInteger, MBNNWaypointType);

@interface MBNNWaypoint ()
- (nonnull instancetype)initWithName:(nonnull NSString *)name
                            location:(CLLocationCoordinate2D)location
                            distance:(nullable NSNumber *)distance
                            metadata:(nullable NSString *)metadata
                              target:(nullable MBXCoordinate2D *)target
                                type:(MBNNWaypointType)type
                            timeZone:(nullable MBNNTimeZone *)timeZone;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Time zone info
 */
@property (nonatomic, readonly, nullable) MBNNTimeZone *timeZone;

@end
