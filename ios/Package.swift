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

let navNativeVersion = "324.25.0"
let navNativeChecksum = "placeholder_run_swift_build_to_get_real_checksum"
let mapsVersion: Version = "11.25.0"
let commonVersion: Version = "24.25.0"
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
