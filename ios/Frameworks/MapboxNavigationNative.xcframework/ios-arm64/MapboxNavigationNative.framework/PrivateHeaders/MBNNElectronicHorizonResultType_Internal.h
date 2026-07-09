// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Electronic horizon result type.
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNElectronicHorizonResultType)
{
    /** We start moving on new most probable path */
    MBNNElectronicHorizonResultTypeInitial,
    /** We moving on previous most probable path */
    MBNNElectronicHorizonResultTypeUpdate,
    /** EH is not available due to being in off-road, fallback or uncertain state */
    MBNNElectronicHorizonResultTypeNotAvailable
} NS_SWIFT_NAME(ElectronicHorizonResultType);

NSString* MBNNElectronicHorizonResultTypeToString(MBNNElectronicHorizonResultType electronic_horizon_result_type);
