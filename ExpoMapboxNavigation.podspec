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
  # MapboxNavigationCore/UIKit frameworks require it.
  s.platforms      = { :ios => '15.1' }
  s.swift_version  = '5.9'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }

  s.dependency 'ExpoModulesCore'

  # ── Sibling Mapbox pods — all come from CocoaPods (via @rnmapbox/maps) ────
  #
  # Our vendored MapboxNavigationCore/MapboxDirections frameworks' private
  # Swift interfaces (.private.swiftinterface) import MapboxCommon_Private
  # and Turf internally. Without an explicit CocoaPods dependency declared
  # here, CocoaPods doesn't wire up the module/header search paths needed
  # for Swift to resolve those imports from OUR target when compiling
  # against our vendored frameworks — even though @rnmapbox/maps already
  # installs these same pods elsewhere in the project.
  #
  # ⚠️ REVERTED from vendoring MapboxMaps.xcframework directly (from
  # mapbox-maps-ios-binary) + a Podfile-level override forcing
  # @rnmapbox/maps to use that copy. That approach targeted a real,
  # confirmed root cause (mapbox/mapbox-maps-ios#1669: CocoaPods-trunk
  # MapboxMaps lacks BUILD_LIBRARY_FOR_DISTRIBUTION, causing a DYLD
  # missing-symbol crash) — but the override itself broke CocoaPods'
  # automatic `[CP] Copy XCFrameworks` build phase generation for that
  # specific pod (confirmed via a real build log: the phase ran for every
  # sibling Mapbox pod except the overridden one), causing a persistent
  # "no such module 'MapboxMaps'" compile error that several rounds of
  # manual xcconfig patching couldn't fully resolve.
  #
  # Reverted to a plain `s.dependency` here instead, relying on
  # CocoaPods' own natural pod-name deduplication with @rnmapbox/maps'
  # own dependency on 'MapboxMaps' — matching a real, confirmed-working
  # reference implementation
  # (stefanpavlovic-tech/react-native-mapbox-navigation, commit 6ede7e1 —
  # "Verified: headless xcodebuild link BUILD SUCCEEDED, zero duplicate
  # symbols, all 10 Mapbox/nav frameworks embedded exactly once").
  #
  # ⚠️ HONEST CAVEAT: that reference implementation's own verification
  # was about the BUILD succeeding, not about confirmed crash-free
  # behavior on a real device. The original DYLD "Symbol not found:
  # GestureType.singleTap" launch crash this whole investigation started
  # from was never confirmed fixed by this specific change, and this
  # reversion could plausibly reintroduce it. This reversion's goal is
  # narrower: get past the currently-blocking compile-time error first.
  #
  # ⚠️ MAINTAINER: this value MUST be updated together with
  # MAPBOX_NAV_VERSION in ios/fetch-xcframeworks.sh, the consuming app's
  # `RNMapboxMapsVersion`, and s.platforms above, whenever the vendored
  # SDK version is bumped — see "Upgrading the vendored iOS SDK version"
  # in README.md.
  s.dependency 'MapboxCommon'
  s.dependency 'MapboxCoreMaps'
  s.dependency 'MapboxMaps', '11.20.0'
  s.dependency 'Turf'

  # ── iOS: Mapbox Navigation SDK v3, via VENDORED XCFRAMEWORKS ───────────────
  #
  # Mapbox Navigation SDK v3 is distributed as SOURCE CODE via SPM only — it
  # has no CocoaPods support. So: ios/Frameworks/*.xcframework are fetched
  # ONCE (see ios/fetch-xcframeworks.sh — downloads official precompiled
  # binaries from mapbox-navigation-ios-build-artifacts) and committed to
  # this package.
  #
  # IMPORTANT — do NOT vendor MapboxMaps/MapboxCommon/MapboxCoreMaps/Turf
  # here. @rnmapbox/maps already installs those via CocoaPods, and
  # MapboxNavigationCore.xcframework is built to link against that SAME
  # version (kept in sync — see ios/fetch-xcframeworks.sh for the
  # version-alignment requirement). Vendoring a second copy of those
  # specific frameworks would reintroduce duplicate-symbol errors, per the
  # exact guidance a Mapbox engineer gave for this same scenario on
  # mapbox/mapbox-navigation-ios#4703.
  s.vendored_frameworks = Dir.glob(File.join(__dir__, 'ios/Frameworks/*.xcframework')).map { |f| f.sub("#{__dir__}/", '') }

  # ⚠️ CHANGED from a recursive glob (`ios/**/*.{swift,h,m,mm}`) to a
  # non-recursive one, matching a real, confirmed-working reference
  # implementation's own fix for a real bug: a recursive source_files
  # glob COMBINED WITH an exclude_files pattern targeting
  # ios/Frameworks/*.xcframework/**/*.h can cause CocoaPods to strip the
  # vendored frameworks themselves too (it applies exclude_files broadly),
  # producing a "no such module" error for a vendored framework —
  # confirmed by stefanpavlovic-tech/react-native-mapbox-navigation's own
  # commit 6ede7e1 fixing exactly this. This project's own Swift/ObjC
  # source all lives directly under ios/ (not nested in subdirectories),
  # so a non-recursive glob covers everything needed without ever
  # descending into ios/Frameworks/ at all — removing the need for
  # exclude_files entirely.
  s.source_files = 'ios/*.{swift,h,m,mm}'

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

  # ⚠️ CHANGED: this now actually FETCHES the required xcframeworks (via
  # ios/fetch-xcframeworks.sh) instead of just warning that they're
  # missing. ios/Frameworks/*.xcframework is no longer committed to this
  # package's repo or npm tarball (see .gitignore and package.json's
  # "files" — both updated together with this change) — Mapbox's own
  # Product Terms ("1.10. No Redistribution") prohibit redistributing
  # their SDK binaries to third parties who haven't authenticated with
  # their own Mapbox account/token, and this repository is public.
  # Instead, whichever app actually consumes this package fetches these
  # binaries itself, using ITS OWN Mapbox DOWNLOADS:READ token, right here
  # at `pod install` time — matching the same approach used by other
  # public Mapbox Navigation + React Native packages (e.g.
  # pawan-pk/react-native-mapbox-navigation).
  #
  # Requires: network access, and a valid ~/.netrc with Mapbox
  # DOWNLOADS:READ credentials — written automatically by this package's
  # own Expo config plugin from its `downloadsToken` option (see
  # plugin/src/index.js), which runs during `expo prebuild`, always
  # before `pod install`.
  s.prepare_command = <<-CMD
    if [ -z "$(ls -A ios/Frameworks 2>/dev/null)" ]; then
      echo "[ExpoMapboxNavigation] ios/Frameworks is empty — fetching Mapbox Navigation xcframeworks now (this can take several minutes)..."
      if [ ! -f "ios/fetch-xcframeworks.sh" ]; then
        echo "error: [ExpoMapboxNavigation] ios/fetch-xcframeworks.sh not found — cannot fetch required binaries."
        exit 1
      fi
      chmod +x ios/fetch-xcframeworks.sh
      if ! ios/fetch-xcframeworks.sh; then
        echo ""
        echo "error: [ExpoMapboxNavigation] Failed to fetch required Mapbox Navigation xcframeworks."
        echo "       This requires a valid Mapbox DOWNLOADS:READ token in ~/.netrc and network access."
        echo "       Make sure the downloadsToken option is set in your app.json config plugin entry."
        echo "       See this package's README for the exact syntax."
        exit 1
      fi
    fi
  CMD
end
