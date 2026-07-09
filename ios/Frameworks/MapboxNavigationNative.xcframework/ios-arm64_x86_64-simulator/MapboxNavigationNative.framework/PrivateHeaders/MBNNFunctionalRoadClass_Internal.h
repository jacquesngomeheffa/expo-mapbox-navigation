// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Functional road class
 * See for details: https://wiki.openstreetmap.org/wiki/Key:highway
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNFunctionalRoadClass)
{
    MBNNFunctionalRoadClassMotorway,
    MBNNFunctionalRoadClassTrunk,
    MBNNFunctionalRoadClassPrimary,
    MBNNFunctionalRoadClassSecondary,
    MBNNFunctionalRoadClassTertiary,
    MBNNFunctionalRoadClassUnclassified,
    MBNNFunctionalRoadClassResidential,
    MBNNFunctionalRoadClassServiceOther
} NS_SWIFT_NAME(FunctionalRoadClass);

NSString* MBNNFunctionalRoadClassToString(MBNNFunctionalRoadClass functional_road_class);
