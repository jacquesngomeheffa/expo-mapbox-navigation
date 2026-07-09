// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

@protocol MBNNRDTRdAnnotation;
@protocol MBNNRDTRdLegClosureArray;

/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Mutable part of Route leg information. But this object is immutable.
 */
NS_SWIFT_NAME(RdLegAnnotation)
@protocol MBNNRDTRdLegAnnotation
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Annotation of the route.
 */
- (nonnull id<MBNNRDTRdAnnotation>)annotation;
/**
 * WARNING: This API is not intended for public usage. It can be deleted or changed without any notice.
 * Live-traffic closures along the route.
 */
- (nonnull id<MBNNRDTRdLegClosureArray>)closures;
@end
