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

  # Mapbox Navigation SDK v3 requires iOS 14+
  s.platforms      = { :ios => '14.0' }
  s.swift_version  = '5.9'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }
  s.static_framework = true

  s.dependency 'ExpoModulesCore'

  # ── iOS: Mapbox Navigation SDK v3 via VENDORED XCFRAMEWORKS ───────────────
  #
  # Mapbox Navigation SDK v3 is distributed as SOURCE CODE via SPM only — it
  # has no CocoaPods support (confirmed still true as of mid-2026:
  # https://docs.mapbox.com/ios/navigation/guides/install/ — "CocoaPods
  # support is currently in development"). Trying to make CocoaPods and a
  # live SPM package resolution cooperate at `pod install` time, so that a
  # source-distributed package like MapboxNavigationCore correctly links
  # against its own binary dependency MapboxNavigationNative, turned out to
  # be fundamentally unreliable in this project's setup:
  #
  #   - React Native's own SPM manager (react-native/scripts/cocoapods/
  #     spm.rb) unconditionally wipes any SPM package_references not
  #     declared through its own spm_dependency() API during post_install,
  #     regardless of hook ordering.
  #   - spm_dependency() itself is documented by React Native to cause
  #     "Undefined symbols"/duplicate-symbol errors on statically-linked
  #     Expo modules (see facebook/react-native#47344) — a structural
  #     conflict with ExpoModulesCore, which must be static.
  #   - A Mapbox engineer confirmed on mapbox/mapbox-navigation-ios#4703
  #     that MapboxNavigationCore/MapboxNavigationUIKit are source-only as
  #     of v3.2.0+ (unlike MapboxNavigationNative/MapboxCommon/
  #     MapboxCoreMaps, which remain precompiled binaries) and that the
  #     supported workaround for this exact class of project (a
  #     modularized target consuming the Nav SDK alongside other
  #     pre-existing Mapbox dependencies) is to prebuild local xcframeworks
  #     with `xcodebuild -create-xcframework` or the Scipio tool, then
  #     vendor them directly.
  #
  # So: ios/Frameworks/*.xcframework are prebuilt ONCE (via
  # .github/workflows/build-xcframeworks.yml, on a free GitHub-hosted
  # macOS runner — see that file) and committed to this package. No
  # network access to api.mapbox.com, no SPM package resolution, and none
  # of the CocoaPods/SPM interop machinery above is needed at `pod install`
  # or `xcodebuild` time for any consumer of this pod anymore.
  #
  # IMPORTANT — do NOT vendor MapboxMaps/MapboxCommon/MapboxCoreMaps/Turf
  # here. @rnmapbox/maps already installs those via CocoaPods, and
  # MapboxNavigationCore.xcframework is built to link against that SAME
  # version (kept in sync — see ios/Package.swift and
  # ios/build-xcframeworks.sh for the version-alignment requirement).
  # Vendoring a second copy of those specific frameworks would reintroduce
  # duplicate-symbol errors, per the exact guidance a Mapbox engineer gave
  # for this same scenario in the issue linked above.
  s.vendored_frameworks = Dir.glob(File.join(__dir__, 'ios/Frameworks/*.xcframework'))

  s.source_files = 'ios/**/*.{swift,h,m,mm}'
  s.exclude_files = [
    'ios/Package.swift',
    'ios/build-xcframeworks.sh',
    'ios/Frameworks/*.xcframework/**/*.h',
  ]

  s.pod_target_xcconfig = {
    'DEFINES_MODULE'             => 'YES',
    'SWIFT_COMPILATION_MODE'     => 'wholemodule',
    'IPHONEOS_DEPLOYMENT_TARGET' => '14.0',
  }

  s.prepare_command = <<-CMD
    if [ -z "$(ls -A ios/Frameworks 2>/dev/null)" ]; then
      echo "warning: ios/Frameworks is empty — the xcframeworks have not been built yet."
      echo "         Run the 'Build Mapbox Navigation xcframeworks' GitHub Actions workflow"
      echo "         and merge its output branch before publishing this package."
    fi
  CMD
end
