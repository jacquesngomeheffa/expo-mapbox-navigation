// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Surface type of road
 * See for details: https://wiki.openstreetmap.org/wiki/Key:surface
 */
// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNRoadSurface)
{
    MBNNRoadSurfacePavedSmooth,
    MBNNRoadSurfacePaved,
    MBNNRoadSurfacePavedRough,
    MBNNRoadSurfaceCompacted,
    MBNNRoadSurfaceDirt,
    MBNNRoadSurfaceGravel,
    MBNNRoadSurfacePath,
    MBNNRoadSurfaceImpassable
} NS_SWIFT_NAME(RoadSurface);

NSString* MBNNRoadSurfaceToString(MBNNRoadSurface road_surface);
