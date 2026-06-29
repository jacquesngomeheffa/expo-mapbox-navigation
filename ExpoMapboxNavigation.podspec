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
  s.platforms      = { :ios => '14.0' }
  s.swift_version  = '5.7'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }
  s.static_framework = true

  s.dependency 'ExpoModulesCore'

  # ── iOS: Mapbox Navigation bundled as xcframework ──────────────────────────
  # The Navigation SDK v3 for iOS does not support CocoaPods natively.
  # xcframeworks are bundled directly. See README for how to rebuild them.
  # ────────────────────────────────────────────────────────────────────────────
  s.vendored_frameworks = 'ios/MapboxNavigationUIKit.xcframework',
                          'ios/MapboxNavigationCore.xcframework',
                          'ios/MapboxDirections.xcframework'

  # Mapbox Maps SDK (via CocoaPods — available as of Maps SDK 11.x)
  s.dependency 'MapboxMaps', '>= 11.11.0'

  s.source_files = 'ios/**/*.{swift,m,mm}'

  s.pod_target_xcconfig = {
    'DEFINES_MODULE' => 'YES',
    # 16 KB page alignment for iOS (Apple Silicon + future-proof)
    'OTHER_LDFLAGS' => '-Wl,-dead_strip_dylibs',
    'SWIFT_COMPILATION_MODE' => 'wholemodule'
  }
end
