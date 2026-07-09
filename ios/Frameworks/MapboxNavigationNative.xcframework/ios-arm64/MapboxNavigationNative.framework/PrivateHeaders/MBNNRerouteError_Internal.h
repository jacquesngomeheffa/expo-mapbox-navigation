// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNRouterError;
typedef NS_ENUM(NSInteger, MBNNRerouteErrorType);

NS_SWIFT_NAME(RerouteError)
__attribute__((visibility ("default")))
@interface MBNNRerouteError : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithMessage:(nonnull NSString *)message
                                   type:(MBNNRerouteErrorType)type
                           routerErrors:(nonnull NSArray<MBNNRouterError *> *)routerErrors;

@property (nonatomic, readonly, nonnull, copy) NSString *message;
@property (nonatomic, readonly) MBNNRerouteErrorType type;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * If type is RouterError, will hold the return errors, otherwise empty
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNRouterError *> *routerErrors;


- (BOOL)isEqualToRerouteError:(nonnull MBNNRerouteError *)other;

@end
