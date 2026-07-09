// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

NS_SWIFT_NAME(TilesEndpointConfig)
__attribute__((visibility ("default")))
@interface MBNNTilesEndpointConfig : NSObject

// This class provides custom init which should be called
- (nonnull instancetype)init NS_UNAVAILABLE;

// This class provides custom init which should be called
+ (nonnull instancetype)new NS_UNAVAILABLE;

- (nonnull instancetype)initWithHost:(nonnull NSString *)host
                             dataset:(nonnull NSString *)dataset
minDiffInDaysToConsiderServerVersion:(nullable NSNumber *)minDiffInDaysToConsiderServerVersion;

/**
 * API routing tiles (ART) endpoint address (e.g. https://api.mapbox.com)
 * Could be one of the staging servers or api.mapbox.com (primarily for OSM now)
 * Various servers may have the info from different data providers.
 * If empty, no network requests will be made to ART
 */
@property (nonatomic, readonly, nonnull, copy) NSString *host;

/** Tile dataset to use when querying ART, for example 'mapbox/driving' */
@property (nonatomic, readonly, nonnull, copy) NSString *dataset;

/**
 * Minimum time in days between local version of tiles and latest on the server
 * to consider using the latest version of routing tiles from the server.
 * The parameter is applied only to the case with automatic version switching.
 * The main purpose of the parameter - ability to do update frequency throttling.
 * It also assumes there are regular update on the server side.
 * Default: 0
 */
@property (nonatomic, readonly, nullable) NSNumber *minDiffInDaysToConsiderServerVersion;


- (BOOL)isEqualToTilesEndpointConfig:(nonnull MBNNTilesEndpointConfig *)other;

@end
