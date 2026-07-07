// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

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
