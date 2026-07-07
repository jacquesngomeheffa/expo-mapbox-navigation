// This file is generated and will be overwritten automatically.

#import <MapboxNavigationNative/MBNNBannerComponent.h>

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
@property (nonatomic, readonly, nullable) MBNNShield *shield;
@end
