// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNConfigHandle;
@class MBNNNavigatorConfig;
@class MBNNSettingsProfile;

NS_SWIFT_NAME(ConfigFactory)
__attribute__((visibility ("default")))
@interface MBNNConfigFactory : NSObject

+ (nonnull MBNNConfigHandle *)buildForProfile:(nonnull MBNNSettingsProfile *)profile
                                       config:(nonnull MBNNNavigatorConfig *)config
                                 customConfig:(nonnull NSString *)customConfig __attribute((ns_returns_retained));

@end
