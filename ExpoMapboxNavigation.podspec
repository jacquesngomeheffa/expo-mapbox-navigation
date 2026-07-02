require 'json'

package = JSON.parse(File.read(File.join(__dir__, 'package.json')))

Pod::Spec.new do |s|
  s.name           = 'ExpoMapboxNavigation'
  s.version        = package['version']
  s.summary        = 'Expo module for Mapbox Navigation SDK v3 — Android and iOS'
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

  # ── Sibling Mapbox pods (provided by @rnmapbox/maps, not vendored here) ───
  # Our vendored MapboxNavigationCore/MapboxDirections frameworks' private
  # Swift interfaces (.private.swiftinterface) import MapboxCommon_Private
  # and Turf internally. Without an explicit CocoaPods dependency declared
  # here, CocoaPods doesn't wire up the module/header search paths needed
  # for Swift to resolve those imports from OUR target when compiling
  # against our vendored frameworks — even though @rnmapbox/maps already
  # installs these same pods elsewhere in the project. No version pin here:
  # CocoaPods resolves one version per pod name across the whole Podfile,
  # so this just reuses whatever version @rnmapbox/maps already pulls in
  # rather than risking a second, conflicting constraint.
  s.dependency 'MapboxCommon'
  s.dependency 'MapboxCoreMaps'
  s.dependency 'MapboxMaps'
  s.dependency 'Turf'

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
  #   - Earlier (2024), MapboxNavigationCore/MapboxNavigationUIKit were
  #     source-only (a Mapbox engineer confirmed this on
  #     mapbox/mapbox-navigation-ios#4703 and suggested prebuilding local
  #     xcframeworks with a tool like Scipio as a workaround). Mapbox has
  #     SINCE closed that gap themselves: they now publish a dedicated
  #     repo, mapbox-navigation-ios-build-artifacts, which exposes
  #     MapboxNavigationCore/UIKit (and their own transitive dependencies)
  #     as officially precompiled .xcframework downloads — the same
  #     api.mapbox.com/downloads/v2/... mechanism already used for
  #     MapboxNavigationNative/MapboxCommon/MapboxCoreMaps/Turf throughout
  #     this project. No local compilation is needed for any of it anymore.
  #
  # So: ios/Frameworks/*.xcframework are fetched ONCE (via
  # .github/workflows/build-xcframeworks.yml, on a free GitHub-hosted
  # macOS runner — see that file and ios/fetch-xcframeworks.sh) and
  # committed to this package. No network access to api.mapbox.com, no SPM
  # package resolution, and none of the CocoaPods/SPM interop machinery
  # above is needed at `pod install` or `xcodebuild` time for any consumer
  # of this pod anymore.
  #
  # IMPORTANT — do NOT vendor MapboxMaps/MapboxCommon/MapboxCoreMaps/Turf
  # here. @rnmapbox/maps already installs those via CocoaPods, and
  # MapboxNavigationCore.xcframework is built to link against that SAME
  # version (kept in sync — see ios/fetch-xcframeworks.sh for the
  # version-alignment requirement). Vendoring a second copy of those
  # specific frameworks would reintroduce duplicate-symbol errors, per the
  # exact guidance a Mapbox engineer gave for this same scenario in the
  # issue linked above.
  s.vendored_frameworks = Dir.glob(File.join(__dir__, 'ios/Frameworks/*.xcframework')).map { |f| f.sub("#{__dir__}/", '') }

  s.source_files = 'ios/**/*.{swift,h,m,mm}'
  s.exclude_files = [
    'ios/fetch-xcframeworks.sh',
    'ios/Frameworks/*.xcframework/**/*.h',
  ]

  s.pod_target_xcconfig = {
    'DEFINES_MODULE'             => 'YES',
    'SWIFT_COMPILATION_MODE'     => 'wholemodule',
    'IPHONEOS_DEPLOYMENT_TARGET' => '14.0',
  }

  # ── Avoid the .private.swiftinterface toolchain-version check ─────────────
  # Debug builds default to ENABLE_TESTABILITY = YES (needed for @testable
  # import elsewhere in the project). That setting makes Xcode re-verify our
  # vendored frameworks' .private.swiftinterface (the "testable" textual
  # interface) instead of just linking the precompiled .swiftmodule binary
  # directly — and THAT re-verification step is what triggers Swift's
  # strict "this SDK is not supported by the compiler" check if the Swift
  # compiler that built the vendored xcframeworks differs at all from the
  # one doing the build (a real, standard, well-documented Swift/Xcode
  # check — see forums.swift.org and developer.apple.com/forums threads on
  # this exact error, and mapbox/mapbox-maps-ios#1363 for the same issue
  # with an earlier Mapbox binary distribution). Precompiled binaries built
  # with library evolution enabled (which is how Mapbox ships these) are
  # meant to tolerate a newer consuming compiler without this stricter
  # recheck — disabling testability avoids forcing that recheck in the
  # first place. This is scoped to app (not test) targets; if your project
  # relies on @testable import of your OWN code elsewhere, this setting
  # does not affect that — it only affects whether Xcode treats imports of
  # vendored/third-party frameworks like this one as needing their private
  # interface.
  s.user_target_xcconfig = {
    'ENABLE_TESTABILITY' => 'NO',
  }

  s.prepare_command = <<-CMD
    if [ -z "$(ls -A ios/Frameworks 2>/dev/null)" ]; then
      echo "warning: ios/Frameworks is empty — the xcframeworks have not been built yet."
      echo "         Run the 'Build Mapbox Navigation xcframeworks' GitHub Actions workflow"
      echo "         and merge its output branch before publishing this package."
    fi
  CMD
end
