// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNTollCollectionType);

NS_SWIFT_NAME(TollCollectionInfo)
__attribute__((visibility ("default")))
@interface MBNNTollCollectionInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(nonnull NSString *)id_
                              type:(MBNNTollCollectionType)type
                              name:(nullable NSString *)name;

/** id of the toll collection */
@property (nonatomic, readonly, nonnull, copy) NSString *id;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * type of toll collection point
 */
@property (nonatomic, readonly) MBNNTollCollectionType type;

/** name of the toll */
@property (nonatomic, readonly, nullable, copy) NSString *name;


- (BOOL)isEqualToTollCollectionInfo:(nonnull MBNNTollCollectionInfo *)other;

@end
