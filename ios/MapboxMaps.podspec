Pod::Spec.new do |s|
  # ── Overrides the 'MapboxMaps' CocoaPods trunk pod project-wide ───────────
  #
  # This podspec is NOT autolinked by Expo/React Native — it is deliberately
  # named 'MapboxMaps' (same as the real CocoaPods trunk pod @rnmapbox/maps
  # depends on) and referenced explicitly, by :podspec path, from a Podfile
  # override injected by plugin/src/index.js's `withMapboxMapsPodfileOverride`
  # mod. CocoaPods resolves exactly ONE version/source per pod name across
  # an entire Podfile — an explicit `pod 'MapboxMaps', :podspec => '...'`
  # declaration in the Podfile takes precedence over what any dependency
  # (including @rnmapbox/maps) implicitly requests from trunk. The practical
  # effect: @rnmapbox/maps' own `s.dependency 'MapboxMaps'` resolves to THIS
  # local podspec too, instead of fetching its own separate copy — avoiding
  # a duplicate-symbol crash from having two different MapboxMaps binaries
  # linked into the same app.
  #
  # WHY THIS EXISTS AT ALL: confirmed directly from a Mapbox engineer on
  # their own issue tracker (mapbox/mapbox-maps-ios#1669) — MapboxMaps as
  # distributed via CocoaPods trunk is NOT built with
  # BUILD_LIBRARY_FOR_DISTRIBUTION=YES, so "some symbols are stripped from
  # the binary," causing exactly the DYLD "Symbol not found" launch crash
  # this whole investigation chased for a very long time when a separately
  # compiled framework (our own vendored MapboxNavigationCore.xcframework)
  # depends on it. The real fix is to vendor MapboxMaps.xcframework from
  # Mapbox's own dedicated binary-distribution repo
  # (mapbox-maps-ios-binary — see ios/fetch-xcframeworks.sh), which IS
  # built with that flag — and to make sure that's the ONLY copy of
  # MapboxMaps that ends up in the final app, via this override.
  s.name              = 'MapboxMaps'
  s.version           = '11.20.0'
  s.summary           = 'Vendored MapboxMaps.xcframework overriding CocoaPods trunk to fix a DYLD missing-symbol launch crash.'
  s.description       = 'Vendored MapboxMaps.xcframework (built WITH BUILD_LIBRARY_FOR_DISTRIBUTION=YES via mapbox-maps-ios-binary), overriding CocoaPods trunk project-wide to fix a DYLD missing-symbol launch crash. See ExpoMapboxNavigation.podspec and ios/fetch-xcframeworks.sh for the full investigation this addresses.'
  s.homepage          = 'https://github.com/mapbox/mapbox-maps-ios'
  s.license           = { :type => 'Mapbox Terms of Service', :text => 'See https://www.mapbox.com/legal/tos/' }
  s.author            = 'Mapbox'
  s.platforms         = { :ios => '15.1' }
  s.source             = { :git => 'https://github.com/mapbox/mapbox-maps-ios.git', :tag => "v#{s.version}" }

  # Vendored by ios/fetch-xcframeworks.sh, from mapbox-maps-ios-binary — a
  # separate Mapbox repo dedicated to distributing MapboxMaps as a
  # precompiled binary, distinct from mapbox-navigation-ios-build-artifacts
  # (used for MapboxNavigationCore/UIKit/etc. — see the main
  # ExpoMapboxNavigation.podspec). Same ios/Frameworks/ directory as the
  # rest of this package's vendored binaries — this podspec lives
  # alongside it, one directory up from where the main podspec sits.
  s.vendored_frameworks = 'Frameworks/MapboxMaps.xcframework'

  # Matches how mapbox-maps-ios-binary itself builds this — a dynamic
  # framework, not static. Do NOT change this to true: the whole point of
  # vendoring from this specific channel is that it's built WITH
  # BUILD_LIBRARY_FOR_DISTRIBUTION, which produces a dynamic xcframework;
  # forcing s.static_framework here would not "convert" the underlying
  # binary (which is already compiled) and could reintroduce linkage
  # inconsistencies with @rnmapbox/maps' own expectations for this pod.
  s.static_framework = false

  # FIX for a real, confirmed compile error: "no such module 'MapboxMaps'"
  # in @rnmapbox/maps' own Swift source, on a real device archive build,
  # AFTER confirming (via a Podfile post_install diagnostic) that this
  # pod's own DEFINES_MODULE was already correctly 'YES' — so that was
  # never the actual missing piece.
  #
  # Root cause, confirmed via a real, matching CocoaPods issue
  # (CocoaPods/CocoaPods#10058 — same symptom: a pure-Swift xcframework
  # vendored via `vendored_frameworks`, correctly appearing in the
  # consuming target's Library/Framework Search Paths, but Xcode still
  # can't find its .swiftmodule) and CocoaPods' own official
  # documentation for `Pod::Target::BuildSettings` (confirming
  # `PODS_XCFRAMEWORKS_BUILD_DIR` — "the configuration intermediate
  # frameworks directory used for building pod targets that contain
  # vendored xcframeworks" — is a real, intentional CocoaPods mechanism,
  # not a workaround): CocoaPods sets up FRAMEWORK_SEARCH_PATHS/
  # HEADER_SEARCH_PATHS automatically for vendored xcframeworks, but NOT
  # SWIFT_INCLUDE_PATHS — the specific setting the Swift compiler (not
  # the linker) actually uses to resolve `import ModuleName` for a
  # dependent target's own Swift source files. This is exactly why
  # `rnmapbox-maps`'s own `Array+asExpressions.swift` (`import
  # MapboxMaps`) failed to compile despite CocoaPods otherwise correctly
  # resolving and linking this pod.
  #
  # `s.user_target_xcconfig` (not `s.pod_target_xcconfig`) applies this
  # to every target that DEPENDS ON this pod — i.e. `rnmapbox-maps` and
  # this project's own `ExpoMapboxNavigation` target — not to this pod's
  # own (nonexistent — vendored_frameworks only, no source) build.
  s.user_target_xcconfig = {
    'SWIFT_INCLUDE_PATHS' => '"${PODS_XCFRAMEWORKS_BUILD_DIR}/MapboxMaps"',
  }
end
