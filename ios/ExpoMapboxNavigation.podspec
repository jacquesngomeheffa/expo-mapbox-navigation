Pod::Spec.new do |s|
  s.name           = 'ExpoMapboxNavigation'
  s.version        = '1.0.1'
  s.summary        = 'Expo module for Mapbox turn-by-turn navigation — iOS & Android'
  s.homepage       = 'https://github.com/YOUR_GITHUB/expo-mapbox-navigation'
  s.license        = { :type => 'MIT' }
  s.authors        = { 'Your Name' => 'you@example.com' }
  s.platforms      = { :ios => '14.0' }
  s.swift_version  = '5.7'
  s.source         = { :git => '' }

  s.static_framework = true

  s.dependency 'ExpoModulesCore'
  # MapboxNavigationUIKit + MapboxNavigationCore are bundled as .xcframework
  # See README for how to build them from source

  s.source_files   = '*.{swift,h,m,mm,cpp}'
  s.preserve_paths = 'Frameworks/**/*'

  s.pod_target_xcconfig = {
    'FRAMEWORK_SEARCH_PATHS' => '$(inherited) $(PODS_TARGET_SRCROOT)/Frameworks/**',
    'OTHER_LDFLAGS' => '$(inherited) -framework MapboxNavigationUIKit -framework MapboxNavigationCore -framework MapboxNavigationNative -framework MapboxDirections'
  }
end
