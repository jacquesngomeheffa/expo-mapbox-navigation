// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MBNNAmenityType);

NS_SWIFT_NAME(Amenity)
__attribute__((visibility ("default")))
@interface MBNNAmenity : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithType:(MBNNAmenityType)type
                                name:(nullable NSString *)name
                               brand:(nullable NSString *)brand;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * type of amenity
 */
@property (nonatomic, readonly) MBNNAmenityType type;

/** name of amenity */
@property (nonatomic, readonly, nullable, copy) NSString *name;

/** brand name of amenity */
@property (nonatomic, readonly, nullable, copy) NSString *brand;


- (BOOL)isEqualToAmenity:(nonnull MBNNAmenity *)other;

@end
