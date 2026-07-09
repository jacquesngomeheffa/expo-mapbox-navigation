// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Describes a road shield information.
 */
NS_SWIFT_NAME(RdRoadShield)
__attribute__((visibility ("default")))
@interface MBNNRDTRdRoadShield : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithBaseUrl:(nonnull NSString *)baseUrl
                             displayRef:(nonnull NSString *)displayRef
                                   name:(nonnull NSString *)name
                              textColor:(nonnull NSString *)textColor;

/** The base url for a shield image */
@property (nonatomic, readonly, nonnull, copy) NSString *baseUrl;

/** The shield display reference */
@property (nonatomic, readonly, nonnull, copy) NSString *displayRef;

/** The shield text */
@property (nonatomic, readonly, nonnull, copy) NSString *name;

/** The shield text color */
@property (nonatomic, readonly, nonnull, copy) NSString *textColor;


- (BOOL)isEqualToRdRoadShield:(nonnull MBNNRDTRdRoadShield *)other;

@end
