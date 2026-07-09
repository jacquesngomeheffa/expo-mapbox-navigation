// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNLocalizedString;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Japan-specific Interchange info, refers to an expressway entrance and exit, e.g.  Wangannarashino IC
 */
NS_SWIFT_NAME(IcInfo)
__attribute__((visibility ("default")))
@interface MBNNIcInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(nonnull NSString *)id_
                              name:(nonnull NSArray<MBNNLocalizedString *> *)name;

/** id of junction */
@property (nonatomic, readonly, nonnull, copy) NSString *id;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Language order is not guaranteed
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNLocalizedString *> *name;


- (BOOL)isEqualToIcInfo:(nonnull MBNNIcInfo *)other;

@end
