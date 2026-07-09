// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * @brief A structure to hold testing-related context information.
 *
 * The `TestingContext` structure is designed to provide runtime information
 * about the test vehicle and project for testing purposes.
 *
 * @note This structure is intended strictly for testing and debugging purposes.
 * Including sensitive information such as VINs or internal project names in
 * production environments is not recommended.
 */
NS_SWIFT_NAME(TestingContext)
__attribute__((visibility ("default")))
@interface MBNNTestingContext : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithVehicleName:(nonnull NSString *)vehicleName
                                projectName:(nonnull NSString *)projectName;

@property (nonatomic, readonly, nonnull, copy) NSString *vehicleName;
@property (nonatomic, readonly, nonnull, copy) NSString *projectName;

- (BOOL)isEqualToTestingContext:(nonnull MBNNTestingContext *)other;

@end
