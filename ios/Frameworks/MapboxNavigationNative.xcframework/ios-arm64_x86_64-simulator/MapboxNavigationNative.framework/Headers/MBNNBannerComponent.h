// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNShield;
typedef NS_ENUM(NSInteger, MBNNBannerComponentSubType);

NS_SWIFT_NAME(BannerComponent)
__attribute__((visibility ("default")))
@interface MBNNBannerComponent : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;


@property (nonatomic, readonly, nonnull, copy) NSString *type;
@property (nonatomic, readonly, nonnull, copy) NSString *text;
@property (nonatomic, readonly, nullable, copy) NSString *abbr;
@property (nonatomic, readonly, nullable) NSNumber *abbrPriority;
@property (nonatomic, readonly, nullable, copy) NSString *imageBaseUrl;
@property (nonatomic, readonly, nullable) NSNumber *active;
@property (nonatomic, readonly, nullable, copy) NSArray<NSString *> *directions;
@property (nonatomic, readonly, nullable, copy) NSString *activeDirection;
/** The full url for a guidance view image */
@property (nonatomic, readonly, nullable, copy) NSString *imageURL;

/** Provides more context about the component guidance view that may help in visual markup and display choices. */
@property (nonatomic, readonly, nullable) NSNumber *subType;


- (BOOL)isEqualToBannerComponent:(nonnull MBNNBannerComponent *)other;

@end
