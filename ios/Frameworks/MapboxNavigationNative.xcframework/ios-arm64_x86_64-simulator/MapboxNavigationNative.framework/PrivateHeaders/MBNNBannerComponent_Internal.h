// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNBannerComponent.h>

@class MBNNShield;
typedef NS_ENUM(NSInteger, MBNNBannerComponentSubType);

@interface MBNNBannerComponent ()
- (nonnull instancetype)initWithType:(nonnull NSString *)type
                                text:(nonnull NSString *)text
                                abbr:(nullable NSString *)abbr
                        abbrPriority:(nullable NSNumber *)abbrPriority
                        imageBaseUrl:(nullable NSString *)imageBaseUrl
                              active:(nullable NSNumber *)active
                          directions:(nullable NSArray<NSString *> *)directions
                     activeDirection:(nullable NSString *)activeDirection
                            imageURL:(nullable NSString *)imageURL
                             subType:(nullable NSNumber *)subType
                              shield:(nullable MBNNShield *)shield;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Provides shield info for type=icon
 */
@property (nonatomic, readonly, nullable) MBNNShield *shield;

@end
