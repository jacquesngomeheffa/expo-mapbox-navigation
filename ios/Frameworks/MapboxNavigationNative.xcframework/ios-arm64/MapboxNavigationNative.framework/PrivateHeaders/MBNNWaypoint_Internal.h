// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNWaypoint.h>

@interface MBNNWaypoint ()
- (nonnull instancetype)initWithName:(nonnull NSString *)name
                            location:(CLLocationCoordinate2D)location
                            distance:(nullable NSNumber *)distance
                            metadata:(nullable NSString *)metadata
                              target:(nullable MBXCoordinate2D *)target
                                type:(MBNNWaypointType)type
                            timeZone:(nullable MBNNTimeZone *)timeZone;
@property (nonatomic, readonly, nullable) MBNNTimeZone *timeZone;
@end
