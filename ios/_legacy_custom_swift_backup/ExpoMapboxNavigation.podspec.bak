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

  # FIX: reverted from '15.1' back to '13.4'.
  #
  # '15.1' was added earlier in this investigation based on a real build
  # error ("compiling for iOS 14.0, but module 'MapboxMaps' has a minimum
  # deployment target of iOS 15.1"), together with a forced
  # IPHONEOS_DEPLOYMENT_TARGET override below. But youssefhenna/
  # expo-mapbox-navigation (this package's origin project, confirmed
  # working against this exact MapboxMaps version) uses '13.4' — no
  # special-casing at all. That earlier error was real, but its root cause
  # was never conclusively pinned down beyond "something forces a lower
  # deployment target back down" — it's entirely possible the actual cause
  # was specific to whichever xcframework version/build was in place at
  # the time, not a structural requirement of this podspec itself. Since
  # youssefhenna's own simpler podspec is a known-working baseline (this
  # project's origin, actually run in production) and ours has crashed
  # twice despite carrying this extra complexity, reverting to match that
  # baseline as closely as possible takes priority over defending
  # additions this project made to its own theories along the way. If a
  # deployment-target-related build failure reappears, re-add
  # IPHONEOS_DEPLOYMENT_TARGET deliberately at that point, informed by the
  # actual new error — not preemptively.
  s.platforms      = { :ios => '13.4' }
  s.swift_version  = '5.9'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }
  s.static_framework = true

  s.dependency 'ExpoModulesCore'

  # FIX: removed `MapboxCommon`/`MapboxCoreMaps` as explicit dependencies.
  #
  # These were added on the theory that our vendored frameworks' private
  # Swift interfaces needed them declared here for CocoaPods to wire up
  # module/header search paths correctly — but this was never actually
  # confirmed against a real build; it was a plausible-sounding
  # justification added preemptively. youssefhenna/expo-mapbox-navigation
  # (this package's origin project, confirmed working) declares only
  # `MapboxMaps` and `Turf` — nothing else — and its build succeeds.
  # Matching that known-working baseline exactly, rather than keeping
  # unverified extra dependencies this project added to its own theories.
  #
  # `MapboxMaps` stays pinned to an EXACT version here (not left
  # unconstrained, and not read from an ENV var / plugin option the way
  # youssefhenna's own podspec does it) — see the reasoning below.
  #
  # Root cause of a real production launch crash (DYLD "Symbol not found:
  # GestureType.singleTap", confirmed via crash report): with no
  # constraint here, CocoaPods was free to resolve MapboxMaps to whatever
  # satisfied every OTHER pod's constraint across this project's full
  # ~184-pod graph — not guaranteed to be the exact same build our
  # vendored MapboxNavigationCore.xcframework was linked against at
  # Mapbox's own build time, even at a matching version *number*.
  #
  # 11.11.0 is the exact version confirmed required by MapboxNavigationCore
  # 3.8.0 — directly from a real screenshot of mapbox-navigation-ios's own
  # CHANGELOG.md ("## 3.8.0" / "Packaging" / "MapboxNavigationCore now
  # requires MapboxMaps v11.11.0" / "MapboxNavigationNative v324.0.0"), not
  # assumed. This is the pairing youssefhenna/expo-mapbox-navigation
  # documents as actually developed and tested against — not just
  # theoretically compatible per release notes, unlike 3.11.0/11.14.0
  # (used in 3.1.6/Unreleased), which crashed identically to this
  # package's very first crash (3.8.2/11.11.0) despite matching Mapbox's
  # own stated requirement exactly. This is intentionally NOT sourced from
  # a plugin option or environment variable: unlike the Android-side
  # `mapboxMapsVersion` option (see plugin/src/index.js), letting a
  # consumer override this to an arbitrary value would defeat the entire
  # point — it would let someone accidentally request a MapboxMaps version
  # incompatible with THIS exact npm package version's vendored binaries,
  # recreating the very crash this fix closes.
  #
  # ⚠️ MAINTAINER: this value MUST be updated together with
  # MAPBOX_MAPS_VERSION in ios/fetch-xcframeworks.sh, and with
  # MAPBOX_NAV_VERSION there, whenever the vendored SDK version is bumped —
  # see "Upgrading the vendored iOS SDK version" in README.md.
  s.dependency 'MapboxMaps', '11.11.0'
  s.dependency 'Turf', '~> 4.0.0'

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
  # Matches youssefhenna/expo-mapbox-navigation's own podspec — ensures
  # vendored frameworks' headers survive CocoaPods' file processing
  # alongside the exclude_files rule above.
  s.preserve_paths = [
    'ios/Frameworks/*.xcframework',
    'ios/**/*.h',
    'ios/Frameworks/*.xcframework/**/*.h',
  ]

  s.pod_target_xcconfig = {
    'DEFINES_MODULE'             => 'YES',
    'SWIFT_COMPILATION_MODE'     => 'wholemodule',
    'OTHER_SWIFT_FLAGS'          => '$(inherited)',
  }

  s.prepare_command = <<-CMD
    if [ -z "$(ls -A ios/Frameworks 2>/dev/null)" ]; then
      echo "warning: ios/Frameworks is empty — the xcframeworks have not been built yet."
      echo "         Run the 'Build Mapbox Navigation xcframeworks' GitHub Actions workflow"
      echo "         and merge its output branch before publishing this package."
    fi
  CMD
end
