// swift-tools-version:5.9
// This Package.swift is used ONLY to download and build xcframeworks for
// the ExpoMapboxNavigation module's vendored_frameworks in the podspec.
// It is NOT the main entry point for Expo/CocoaPods — that is
// ExpoMapboxNavigation.podspec.
//
// Based on the approach by youssefhenna/expo-mapbox-navigation (the only
// community package proven to work in production EAS iOS builds with the
// Mapbox Navigation SDK v3, which is SPM-only and has no CocoaPods support).
//
// The xcframeworks produced by building this package are vendored directly
// into the npm package (ios/Frameworks/*.xcframework), so no network access
// to api.mapbox.com is needed at `pod install` time — only the .netrc file
// is required during `npm install` / package download, NOT during the build.

import PackageDescription

// IMPORTANT: these three versions must stay aligned with whatever
// @rnmapbox/maps installs via CocoaPods for MapboxCommon/MapboxCoreMaps/
// MapboxMaps/Turf in THIS project (check the `pod install` log for
// "Installing MapboxCommon (...)" etc). A mismatch here means
// MapboxNavigationCore.xcframework is compiled against a different
// MapboxCommon ABI than what's actually linked into the app, which can
// cause symbol/runtime issues even if the build succeeds. As of writing,
// this project pins mapboxMapsVersion "11.11.0", which per Mapbox's own
// compatibility table corresponds to Navigation SDK ~3.8.x and
// MapboxCommon ~24.11.x.
let navNativeVersion = "324.0.5"
let navNativeChecksum = "placeholder_run_swift_build_to_get_real_checksum"
let mapsVersion: Version = "11.11.0"
let commonVersion: Version = "24.11.0"
let mapboxApiDownloads = "https://api.mapbox.com/downloads/v2"

let package = Package(
    name: "MapboxNavigation",
    defaultLocalization: "en",
    platforms: [.iOS(.v14)],
    products: [
        .library(name: "MapboxNavigationUIKit",  targets: ["MapboxNavigationUIKit"]),
        .library(name: "MapboxNavigationCore",   targets: ["MapboxNavigationCore"]),
    ],
    dependencies: [
        .package(url: "https://github.com/mapbox/mapbox-maps-ios.git",   exact: mapsVersion),
        .package(url: "https://github.com/mapbox/mapbox-common-ios.git", exact: commonVersion),
        .package(url: "https://github.com/mapbox/turf-swift.git",        exact: "4.0.0"),
    ],
    targets: [
        .target(
            name: "MapboxNavigationUIKit",
            dependencies: ["MapboxNavigationCore"],
            exclude: ["Info.plist"],
            resources: [
                .copy("Resources/MBXInfo.plist"),
                .copy("Resources/PrivacyInfo.xcprivacy"),
            ]
        ),
        .target(name: "_MapboxNavigationHelpers"),
        .target(
            name: "MapboxNavigationCore",
            dependencies: [
                .product(name: "MapboxCommon", package: "mapbox-common-ios"),
                "MapboxNavigationNative",
                "MapboxDirections",
                "_MapboxNavigationHelpers",
                .product(name: "MapboxMaps", package: "mapbox-maps-ios"),
            ],
            resources: [.process("Resources")]
        ),
        .target(
            name: "MapboxDirections",
            dependencies: [.product(name: "Turf", package: "turf-swift")]
        ),
        .binaryTarget(
            name: "MapboxNavigationNative",
            url: "\(mapboxApiDownloads)/dash-native/releases/ios/packages/\(navNativeVersion)/MapboxNavigationNative.xcframework.zip",
            checksum: navNativeChecksum
        ),
    ]
)
