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

  # Confirmed directly from a Mapbox engineer on their own issue tracker
  # (mapbox/mapbox-maps-ios#1669): MapboxMaps 11.20.0 requires iOS 15.1 —
  # our own target must match or exceed that, since our vendored
  # MapboxNavigationCore/UIKit/MapboxMaps frameworks all require it.
  s.platforms      = { :ios => '15.1' }
  s.swift_version  = '5.9'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }

  s.dependency 'ExpoModulesCore'

  # ── Sibling Mapbox pods — MapboxCommon/MapboxCoreMaps/Turf still come
  #    from CocoaPods (via @rnmapbox/maps); MapboxMaps does NOT anymore ────
  #
  # Our vendored MapboxNavigationCore/MapboxDirections frameworks' private
  # Swift interfaces (.private.swiftinterface) import MapboxCommon_Private
  # and Turf internally. Without an explicit CocoaPods dependency declared
  # here, CocoaPods doesn't wire up the module/header search paths needed
  # for Swift to resolve those imports from OUR target when compiling
  # against our vendored frameworks — even though @rnmapbox/maps already
  # installs these same pods elsewhere in the project.
  #
  # `MapboxMaps` is intentionally NOT declared here anymore. It is vendored
  # directly (see s.vendored_frameworks below, and ios/MapboxMaps.podspec /
  # plugin/src/index.js for the Podfile override that makes @rnmapbox/maps'
  # own dependency resolve to that same vendored copy) — confirmed directly
  # from a Mapbox engineer on their own issue tracker
  # (mapbox/mapbox-maps-ios#1669): MapboxMaps as distributed via CocoaPods
  # trunk is built WITHOUT BUILD_LIBRARY_FOR_DISTRIBUTION=YES, so "some
  # symbols are stripped from the binary" — causing a real, confirmed DYLD
  # "Symbol not found: GestureType.singleTap" launch crash whenever a
  # separately-compiled framework (our own vendored MapboxNavigationCore)
  # depends on it, regardless of version pairing or which channel built
  # MapboxNavigationCore itself (proven with real crash-report binary
  # UUIDs — even a from-source Scipio build hit this identically). The fix
  # is to vendor MapboxMaps from mapbox-maps-ios-binary instead — a
  # separate, official Mapbox repo built WITH that flag — and to make sure
  # it's the only copy of MapboxMaps that ends up in the app (see the
  # Podfile override).
  #
  # ⚠️ MAINTAINER: MAPBOX_NAV_VERSION/MAPBOX_MAPS_VERSION in
  # ios/fetch-xcframeworks.sh, ios/MapboxMaps.podspec's own `s.version`,
  # and s.platforms above must all be updated together whenever the
  # vendored SDK version is bumped — see "Upgrading the vendored iOS SDK
  # version" in README.md.
  s.dependency 'MapboxCommon'
  s.dependency 'MapboxCoreMaps'
  s.dependency 'Turf'

  # ── iOS: Mapbox Navigation SDK v3 + MapboxMaps, via VENDORED XCFRAMEWORKS ──
  #
  # Mapbox Navigation SDK v3 is distributed as SOURCE CODE via SPM only — it
  # has no CocoaPods support. So: ios/Frameworks/*.xcframework are fetched
  # ONCE (see ios/fetch-xcframeworks.sh — downloads official precompiled
  # binaries from mapbox-navigation-ios-build-artifacts for the
  # Navigation-specific frameworks, and from mapbox-maps-ios-binary for
  # MapboxMaps itself) and committed to this package.
  #
  # MapboxMaps.xcframework is fetched into this same ios/Frameworks/
  # directory (see ios/fetch-xcframeworks.sh) but is deliberately EXCLUDED
  # from this glob — it's vendored exclusively by the separate
  # ios/MapboxMaps.podspec (pod name 'MapboxMaps', not 'ExpoMapboxNavigation'),
  # activated via the Podfile override in plugin/src/index.js. Letting
  # BOTH this pod's own vendored_frameworks AND the separate MapboxMaps
  # pod vendor the exact same physical .xcframework file would link it
  # twice into the final app — reintroducing the exact duplicate-symbol
  # problem this whole architecture exists to avoid. See the comment on
  # the removed `s.dependency 'MapboxMaps'` line above for the full
  # reasoning on why MapboxMaps is handled this way at all.
  s.vendored_frameworks = Dir.glob(File.join(__dir__, 'ios/Frameworks/*.xcframework'))
    .reject { |f| File.basename(f) == 'MapboxMaps.xcframework' }
    .map { |f| f.sub("#{__dir__}/", '') }

  s.source_files = 'ios/**/*.{swift,h,m,mm}'
  s.exclude_files = [
    'ios/fetch-xcframeworks.sh',
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
