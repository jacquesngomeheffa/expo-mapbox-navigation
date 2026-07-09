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

  # MapboxMaps 11.11.0 (installed by @rnmapbox/maps) has a minimum
  # deployment target of iOS 15.1 — confirmed directly from a real build
  # error: "compiling for iOS 14.0, but module 'MapboxMaps' has a minimum
  # deployment target of iOS 15.1". Our own target must match or exceed
  # that, since our vendored MapboxNavigationCore/UIKit frameworks import
  # MapboxMaps internally (@_spi imports visible in their private Swift
  # interfaces). iOS 14.0 was carried over from early in this package's
  # history and never actually re-validated against MapboxMaps' real
  # minimum once the SDK version was pinned to 11.11.0.
  s.platforms      = { :ios => '15.1' }
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
  # installs these same pods elsewhere in the project.
  #
  # FIX: `MapboxMaps` now pinned to an EXACT version, not left unconstrained.
  # Root cause of a real production launch crash (DYLD "Symbol not found:
  # GestureType.singleTap", confirmed via crash report), even after
  # RNMapboxMapsVersion was correctly set to match: with no constraint here
  # at all, CocoaPods was free to resolve MapboxMaps to whatever satisfied
  # every OTHER pod's constraint across this project's full ~184-pod graph
  # — which is not guaranteed to be the exact same build our vendored
  # MapboxNavigationCore.xcframework was linked against at Mapbox's own
  # build time, even at a matching version *number*. An explicit, exact
  # constraint here removes that resolution ambiguity entirely, the same
  # way youssefhenna/expo-mapbox-navigation (this package's origin project)
  # pins MapboxMaps explicitly in its own podspec rather than leaving it
  # unconstrained.
  #
  # 11.11.0 is the exact version confirmed required by MapboxNavigationCore
  # 3.8.0 — directly from a real screenshot of mapbox-navigation-ios's own
  # CHANGELOG.md ("## 3.8.0" / "Packaging" / "MapboxNavigationCore now
  # requires MapboxMaps v11.11.0" / "MapboxNavigationNative v324.0.0"), not
  # assumed. This is the pairing youssefhenna/expo-mapbox-navigation (this
  # package's origin project) documents as actually developed and tested
  # against — not just theoretically compatible per release notes, unlike
  # 3.11.0/11.14.0 (used in 3.1.6/Unreleased), which crashed identically to
  # this package's very first crash (3.8.2/11.11.0) despite matching
  # Mapbox's own stated requirement exactly. 3.8.2 (a later patch release)
  # very likely bumped its own required MapboxMaps version past 11.11.0
  # without this being re-verified at the time — a real, unverified
  # assumption that may have been this whole investigation's actual
  # starting mistake. This is intentionally NOT sourced from a plugin
  # option or environment variable: unlike the Android-side
  # `mapboxMapsVersion` option (see plugin/src/index.js), letting a
  # consumer override this to an arbitrary value would defeat the entire
  # point — it would let someone accidentally request a MapboxMaps version
  # incompatible with THIS exact npm package version's vendored binaries,
  # recreating the very crash this fix closes. The iOS Maps version is
  # fixed by which version of this npm package you install, same as
  # MapboxNavigationCore itself (see "iOS Architecture" in README.md) —
  # matching `RNMapboxMapsVersion` in your own app's @rnmapbox/maps config
  # to this exact value remains your responsibility, but this podspec no
  # longer lets CocoaPods silently resolve to something else.
  #
  # ⚠️ MAINTAINER: this value MUST be updated together with
  # MAPBOX_MAPS_VERSION in ios/fetch-xcframeworks.sh, and with
  # MAPBOX_NAV_VERSION there, whenever the vendored SDK version is bumped —
  # see "Upgrading the vendored iOS SDK version" in README.md.
  s.dependency 'MapboxCommon'
  s.dependency 'MapboxCoreMaps'
  s.dependency 'MapboxMaps', '11.11.0'
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
    # CORRECTION (see 2.3.5 changelog): this WAS removed on the theory that
    # s.platforms (above) alone was sufficient and kept as the single
    # source of truth, to reduce the risk of the two declarations going
    # out of sync. That theory was wrong — confirmed by a real build still
    # failing with "compiling for iOS 14.0" even with s.platforms set to
    # 15.1 and this line removed. s.platforms governs CocoaPods'
    # dependency-compatibility validation; it does NOT reliably force the
    # actual IPHONEOS_DEPLOYMENT_TARGET build setting used to invoke the
    # compiler for this target in this project — something (likely the
    # generated Podfile's own default platform declaration) overrides it
    # back down to a lower value otherwise. Both declarations are needed;
    # when bumping the vendored SDK version, update BOTH this value and
    # s.platforms above.
    'IPHONEOS_DEPLOYMENT_TARGET' => '15.1',
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
