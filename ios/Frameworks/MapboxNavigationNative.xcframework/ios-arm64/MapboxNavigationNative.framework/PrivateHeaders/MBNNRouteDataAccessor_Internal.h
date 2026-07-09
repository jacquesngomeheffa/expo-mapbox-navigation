// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@protocol MBNNRDTRdRouteAnnotation;
@protocol MBNNRDTRdRouteData;
@protocol MBNNRouteInterface;

NS_SWIFT_NAME(RouteDataAccessor)
__attribute__((visibility ("default")))
@interface MBNNRouteDataAccessor : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *
 * THIS IS AN EXPERIMENTAL API AND SUBJECT TO CHANGE.
 *
 * A lightweight view object that provides read-only access to parsed Direction API route data structure
 * from which the current object was constructed.
 *
 * Contains only non-mutable data that can't be modified during RouteInterface object lifetime. Currently it's
 * everything except annotations.
 *
 * The returned object contains the strong reference to the RouteInterface object that created it. So this object
 * and any of its children should not be stored for a long time. They are intended to be used for a short time
 * primary only during route data traversing and then be destroyed. They are not intended to be stored on the caller
 * side for long time.
 *
 * \sa https://docs.mapbox.com/api/navigation/directions/
 */
+ (nonnull id<MBNNRDTRdRouteData>)getRouteDataForRoute:(nonnull id<MBNNRouteInterface>)route __attribute((ns_returns_retained));
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 *
 * THIS IS AN EXPERIMENTAL API AND SUBJECT TO CHANGE.
 *
 * The same os getRouteData() but contains the snapshot of the data that can be modified during RouteInterface
 * object lifetime. Currently it's only annotations that RouteInterface can modify during route refresh.
 *
 * NOT IMPLEMENTED YET (!) but should be in the future:
 * - the resulting object doesn't contain the reference to the RouteInterface object that created it. So it can be
 * retained especially for the cases of async data update in main thread.
 * - the returned snapshot contains read-only and non-modifiable data. For retrieving the data that was modified during
 * the last route refresh you should call this method again to obtain the new snapshot of the data.
 */
+ (nonnull id<MBNNRDTRdRouteAnnotation>)getRouteAnnotationForRoute:(nonnull id<MBNNRouteInterface>)route __attribute((ns_returns_retained));

@end
