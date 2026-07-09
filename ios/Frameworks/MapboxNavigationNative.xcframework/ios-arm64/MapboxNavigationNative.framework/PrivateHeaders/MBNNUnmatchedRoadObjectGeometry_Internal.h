// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
@class MBXCoordinate2D;

@class MBNNOpenLR;
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNUnmatchedRoadObjectGeometryType)
{
    MBNNUnmatchedRoadObjectGeometryTypeOpenLR,
    MBNNUnmatchedRoadObjectGeometryTypeCLLocationCoordinate2D,
    MBNNUnmatchedRoadObjectGeometryTypeNSArray
} NS_SWIFT_NAME(UnmatchedRoadObjectGeometryType);

NS_SWIFT_NAME(UnmatchedRoadObjectGeometry)
__attribute__((visibility ("default")))
@interface MBNNUnmatchedRoadObjectGeometry : NSObject

// This class provides factory method which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides factory method which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

+ (nonnull instancetype)fromOpenLR:(nonnull MBNNOpenLR *)value;
+ (nonnull instancetype)fromCLLocationCoordinate2D:(CLLocationCoordinate2D)value;
+ (nonnull instancetype)fromNSArray:(nonnull NSArray<MBXCoordinate2D *> *)value;

- (BOOL)isOpenLR;
- (BOOL)isCLLocationCoordinate2D;
- (BOOL)isNSArray;

- (nonnull MBNNOpenLR *)getOpenLR __attribute((ns_returns_retained));
- (CLLocationCoordinate2D)getCLLocationCoordinate2D;
- (nonnull NSArray<MBXCoordinate2D *> *)getNSArray __attribute((ns_returns_retained));

@property (nonatomic, readonly) MBNNUnmatchedRoadObjectGeometryType type;

@end
