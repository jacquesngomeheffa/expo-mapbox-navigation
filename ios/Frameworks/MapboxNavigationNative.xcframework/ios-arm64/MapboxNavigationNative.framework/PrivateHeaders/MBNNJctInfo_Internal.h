// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNLocalizedString;

NS_SWIFT_NAME(JctInfo)
__attribute__((visibility ("default")))
@interface MBNNJctInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(nonnull NSString *)id_
                              name:(nonnull NSArray<MBNNLocalizedString *> *)name;

/** id of junction */
@property (nonatomic, readonly, nonnull, copy) NSString *id;

@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNLocalizedString *> *name;

- (BOOL)isEqualToJctInfo:(nonnull MBNNJctInfo *)other;

@end
