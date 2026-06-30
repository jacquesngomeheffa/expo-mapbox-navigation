require 'json'

package = JSON.parse(File.read(File.join(__dir__, 'package.json')))

Pod::Spec.new do |s|
  s.name           = 'ExpoMapboxNavigation'
  s.version        = package['version']
  s.summary        = package['description']
  s.description    = package['description']
  s.license        = package['license']
  s.author         = package['author']
  s.homepage       = package['homepage']

  # ── 16 KB / SPM minimum: Mapbox Navigation SDK v3 for iOS requires iOS 14+,
  # but Swift Package Manager dependencies resolved through CocoaPods'
  # spm_dependency() mechanism require iOS 15.1+ as the minimum deployment
  # target (Apple's SPM-via-CocoaPods bridging constraint).
  s.platforms      = { :ios => '15.1' }
  s.swift_version  = '5.9'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }
  s.static_framework = true

  s.dependency 'ExpoModulesCore'

  # ── iOS: Mapbox Navigation SDK v3 via Swift Package Manager ─────────────────
  #
  # IMPORTANT: earlier versions of this podspec referenced prebuilt
  # .xcframework files (MapboxNavigationUIKit.xcframework,
  # MapboxNavigationCore.xcframework, MapboxDirections.xcframework) that were
  # NEVER actually built or shipped in the npm package — this caused
  # "Unimplemented component: ViewManagerAdapter_ExpoMapboxNavigation" at
  # runtime on iOS, since no native module was ever registered.
  #
  # The Mapbox Navigation SDK v3 for iOS is officially distributed ONLY via
  # Swift Package Manager (CocoaPods support is not provided by Mapbox).
  # We use CocoaPods' built-in spm_dependency() helper (available since
  # CocoaPods 1.13+, and exactly the mechanism Expo's own modules use for
  # this exact situation) to pull the SPM package transparently as part of
  # `pod install` / `expo prebuild`. No manual Xcode steps, no vendored
  # binaries to build or maintain.
  spm_dependency(
    s,
    url: 'https://github.com/mapbox/mapbox-navigation-ios.git',
    requirement: { kind: 'upToNextMajorVersion', minimumVersion: '3.24.2' },
    products: ['MapboxNavigationCore', 'MapboxNavigationUIKit', 'MapboxDirections']
  )

  s.source_files = 'ios/**/*.{swift,h,m,mm}'

  s.pod_target_xcconfig = {
    'DEFINES_MODULE' => 'YES',
    'SWIFT_COMPILATION_MODE' => 'wholemodule',
    'IPHONEOS_DEPLOYMENT_TARGET' => '15.1'
  }
end
