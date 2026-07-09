// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Information about the version. Intended for informative use (not to build any logic around it)
 */
NS_SWIFT_NAME(RoadGraphVersionInfo)
__attribute__((visibility ("default")))
@interface MBNNRoadGraphVersionInfo : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithDataset:(nonnull NSString *)dataset
                                version:(nonnull NSString *)version;

@property (nonatomic, readonly, nonnull, copy) NSString *dataset;
@property (nonatomic, readonly, nonnull, copy) NSString *version;

- (BOOL)isEqualToRoadGraphVersionInfo:(nonnull MBNNRoadGraphVersionInfo *)other;

@end
