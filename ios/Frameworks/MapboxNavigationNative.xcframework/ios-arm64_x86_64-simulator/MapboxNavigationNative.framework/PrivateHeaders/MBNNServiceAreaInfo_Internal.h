// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@class MBNNAmenity;
typedef NS_ENUM(NSInteger, MBNNServiceAreaType);

NS_SWIFT_NAME(ServiceAreaInfo)
__attribute__((visibility ("default")))
@interface MBNNServiceAreaInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithId:(nonnull NSString *)id_
                              type:(MBNNServiceAreaType)type
                              name:(nullable NSString *)name
                         amenities:(nonnull NSArray<MBNNAmenity *> *)amenities
                       guideMapUri:(nullable NSString *)guideMapUri;

/** id of service area */
@property (nonatomic, readonly, nonnull, copy) NSString *id;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * type of service area
 */
@property (nonatomic, readonly) MBNNServiceAreaType type;

/** name of the service area */
@property (nonatomic, readonly, nullable, copy) NSString *name;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * array of amenities
 */
@property (nonatomic, readonly, nonnull, copy) NSArray<MBNNAmenity *> *amenities;

/** URI to guide map image */
@property (nonatomic, readonly, nullable, copy) NSString *guideMapUri;


- (BOOL)isEqualToServiceAreaInfo:(nonnull MBNNServiceAreaInfo *)other;

@end
