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

  # RESTORED (from the 3.1.9/3.1.10 youssefhenna-alignment experiment,
  # back to this project's own original approach): MapboxMaps 11.14.0
  # (installed by @rnmapbox/maps) has a minimum deployment target of iOS
  # 15.1 — confirmed directly from a real build error: "compiling for iOS
  # 14.0, but module 'MapboxMaps' has a minimum deployment target of iOS
  # 15.1". Our own target must match or exceed that, since our vendored
  # MapboxNavigationCore/UIKit frameworks import MapboxMaps internally
  # (@_spi imports visible in their private Swift interfaces).
  s.platforms      = { :ios => '15.1' }
  s.swift_version  = '5.9'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }

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
  # `MapboxMaps` pinned to an EXACT version, not left unconstrained. Root
  # cause of a real production launch crash (DYLD "Symbol not found:
  # GestureType.singleTap", confirmed via crash report): with no
  # constraint here, CocoaPods was free to resolve MapboxMaps to whatever
  # satisfied every OTHER pod's constraint across this project's full
  # ~184-pod graph — not guaranteed to be the exact same build our
  # vendored MapboxNavigationCore.xcframework was linked against at
  # Mapbox's own build time, even at a matching version *number*.
  #
  # 11.14.0 is the exact version Mapbox's own v3.11.0 release notes state
  # MapboxNavigationCore requires ("Packaging: MapboxNavigationCore now
  # requires MapboxMaps v11.14.0") — confirmed directly from
  # https://github.com/mapbox/mapbox-navigation-ios/releases/tag/v3.11.0.
  # This is intentionally NOT sourced from a plugin option or environment
  # variable: unlike the Android-side `mapboxMapsVersion` option (see
  # plugin/src/index.js), letting a consumer override this to an arbitrary
  # value would defeat the entire point — it would let someone
  # accidentally request a MapboxMaps version incompatible with THIS exact
  # npm package version's vendored binaries, recreating the very crash
  # this fix closes. The iOS Maps version is fixed by which version of
  # this npm package you install, same as MapboxNavigationCore itself —
  # matching `RNMapboxMapsVersion` in your own app's @rnmapbox/maps config
  # to this exact value remains your responsibility, but this podspec no
  # longer lets CocoaPods silently resolve to something else.
  #
  # ⚠️ MAINTAINER: this value MUST be updated together with
  # MAPBOX_MAPS_VERSION in ios/fetch-xcframeworks-scipio.sh (this
  # version's build method — see below), whenever the vendored SDK
  # version is bumped — see "Upgrading the vendored iOS SDK version" in
  # README.md.
  s.dependency 'MapboxCommon'
  s.dependency 'MapboxCoreMaps'
  s.dependency 'MapboxMaps', '11.14.0'
  s.dependency 'Turf'

  # ── iOS: Mapbox Navigation SDK v3 via VENDORED XCFRAMEWORKS ───────────────
  #
  # Mapbox Navigation SDK v3 is distributed as SOURCE CODE via SPM only — it
  # has no CocoaPods support. So: ios/Frameworks/*.xcframework are built
  # ONCE (see ios/fetch-xcframeworks-scipio.sh — this version's build
  # method, using youssefhenna/expo-mapbox-navigation's own technique: a
  # minimal, hand-written Package.swift + Scipio, built directly against
  # the exact mapbox-maps-ios SPM tag this version targets, rather than
  # downloading Mapbox's own precompiled mapbox-navigation-ios-build-
  # artifacts binaries) and committed to this package.
  #
  # IMPORTANT — do NOT vendor MapboxMaps/MapboxCommon/MapboxCoreMaps/Turf
  # here. @rnmapbox/maps already installs those via CocoaPods, and
  # MapboxNavigationCore.xcframework is built to link against that SAME
  # version (kept in sync — see ios/fetch-xcframeworks-scipio.sh for the
  # version-alignment requirement). Vendoring a second copy of those
  # specific frameworks would reintroduce duplicate-symbol errors, per the
  # exact guidance a Mapbox engineer gave for this same scenario on
  # mapbox/mapbox-navigation-ios#4703.
  s.vendored_frameworks = Dir.glob(File.join(__dir__, 'ios/Frameworks/*.xcframework')).map { |f| f.sub("#{__dir__}/", '') }

  s.source_files = 'ios/**/*.{swift,h,m,mm}'
  s.exclude_files = [
    'ios/fetch-xcframeworks-scipio.sh',
    'ios/Frameworks/*.xcframework/**/*.h',
  ]

  s.pod_target_xcconfig = {
    'DEFINES_MODULE'             => 'YES',
    'SWIFT_COMPILATION_MODE'     => 'wholemodule',
    # s.platforms alone was tried as the single source of truth for the
    # deployment target, on the theory that duplicating it here risked the
    # two declarations going out of sync. That theory was wrong —
    # confirmed by a real build still failing with "compiling for iOS
    # 14.0" even with s.platforms set to 15.1 and this line removed.
    # s.platforms governs CocoaPods' dependency-compatibility validation;
    # it does NOT reliably force the actual IPHONEOS_DEPLOYMENT_TARGET
    # build setting used to invoke the compiler for this target in this
    # project — something (likely the generated Podfile's own default
    # platform declaration) overrides it back down to a lower value
    # otherwise. Both declarations are needed; when bumping the vendored
    # SDK version, update BOTH this value and s.platforms above.
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
  # one doing the build. Precompiled binaries built with library evolution
  # enabled (which is how Mapbox ships these) are meant to tolerate a newer
  # consuming compiler without this stricter recheck — disabling
  # testability avoids forcing that recheck in the first place. This is
  # scoped to app (not test) targets; if your project relies on @testable
  # import of your OWN code elsewhere, this setting does not affect that
  # — it only affects whether Xcode treats imports of vendored/third-party
  # frameworks like this one as needing their private interface.
  s.user_target_xcconfig = {
    'ENABLE_TESTABILITY' => 'NO',
  }

  s.prepare_command = <<-CMD
    if [ -z "$(ls -A ios/Frameworks 2>/dev/null)" ]; then
      echo "warning: ios/Frameworks is empty — the xcframeworks have not been built yet."
      echo "         Run the 'Build Mapbox Navigation xcframeworks (Scipio)' GitHub Actions"
      echo "         workflow and merge its output branch before publishing this package."
    fi
  CMD
end
