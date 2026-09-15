require 'json'

package = JSON.parse(File.read(File.join(__dir__, 'package.json')))

# ── Dynamic iOS SDK version selection (5.0.5+) ────────────────────────────────
# The config plugin's iOS mod writes ios/.mapbox-ios-versions.json during
# `expo prebuild` (always before `pod install`) with the consumer's chosen
# `iosMapboxMapsVersion` / `iosMapboxNavigationVersion` plugin options.
# This podspec only needs the Maps version (the Navigation version is read
# by ios/fetch-xcframeworks.sh itself). If the file is absent or unreadable
# (bare `pod install` without prebuild, older consumers), the hardcoded
# default below keeps the exact pre-5.0.5 behavior. The value is validated
# against a strict semver shape before use — anything else falls back to
# the default rather than injecting arbitrary text into the dependency
# declaration.
# Deliberately a local variable, not a Ruby constant - podspecs can be
# evaluated more than once within a single `pod install`, and re-assigning
# a constant emits "already initialized constant" warnings.
default_mapbox_maps_version = '11.20.2'
mapbox_maps_version = default_mapbox_maps_version
versions_file = File.join(__dir__, 'ios', '.mapbox-ios-versions.json')
if File.exist?(versions_file)
  begin
    requested = JSON.parse(File.read(versions_file))['iosMapboxMapsVersion']
    if requested.is_a?(String) && requested.match?(/\A\d+\.\d+\.\d+\z/)
      mapbox_maps_version = requested
      Pod::UI.puts "[ExpoMapboxNavigation] Using consumer-pinned MapboxMaps #{mapbox_maps_version} (from .mapbox-ios-versions.json)"
    else
      Pod::UI.warn "[ExpoMapboxNavigation] Ignoring invalid iosMapboxMapsVersion #{requested.inspect} - falling back to #{default_mapbox_maps_version}"
    end
  rescue StandardError => e
    Pod::UI.warn "[ExpoMapboxNavigation] Could not read .mapbox-ios-versions.json (#{e.message}) - falling back to MapboxMaps #{default_mapbox_maps_version}"
  end
end

Pod::Spec.new do |s|
  s.name           = 'ExpoMapboxNavigation'
  s.version        = package['version']
  s.summary        = 'Expo module for Mapbox Navigation SDK v3 - Android and iOS'
  s.description    = package['description']
  s.license        = package['license']
  s.author         = package['author']
  s.homepage       = package['homepage']

  # Confirmed directly from a Mapbox engineer on their own issue tracker
  # (mapbox/mapbox-maps-ios#1669): MapboxMaps 11.20.0 requires iOS 15.1 -
  # our own target must match or exceed that, since our vendored
  # MapboxNavigationCore/UIKit frameworks require it.
  s.platforms      = { :ios => '15.1' }
  s.swift_version  = '5.9'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }

  s.dependency 'ExpoModulesCore'

  # -- Sibling Mapbox pods - MapboxMaps comes from CocoaPods (via @rnmapbox/maps) --
  #
  # CORRECTED: an earlier version of this podspec ALSO explicitly declared
  # `s.dependency 'MapboxCommon'`, `'MapboxCoreMaps'`, and `'Turf'` here,
  # on the theory that our vendored MapboxNavigationCore/MapboxDirections
  # frameworks' private Swift interfaces needing these internally meant
  # OUR OWN target needed them declared too. That theory doesn't hold up:
  # this project's own Swift source (ios/ExpoMapboxNavigationModule.swift,
  # ios/ExpoMapboxNavigationView.swift) never directly `import`s
  # MapboxCommon, MapboxCoreMaps, or Turf at all - confirmed by checking
  # the actual files, not assumed. A real, matching CocoaPods issue
  # (CocoaPods/CocoaPods#5264 - "Framework Search Path is missing
  # dependent targets") confirms CocoaPods only needs a transitive
  # dependency explicitly declared on a target that ITSELF imports that
  # module directly; a target that never imports it (like this one, for
  # these three specifically) doesn't need the declaration. `MapboxMaps`
  # itself IS directly imported by ios/ExpoMapboxNavigationView.swift, so
  # it stays declared below - MapboxCommon/MapboxCoreMaps/Turf are
  # resolved automatically as MapboxMaps' own transitive dependencies
  # instead, matching a real, confirmed-working reference implementation
  # (stefanpavlovic-tech/react-native-mapbox-navigation, branch
  # feat/nav-v3-spm - its own podspec declares only `MapboxMaps` the same
  # way, with a comment explicitly noting MapboxCoreMaps/MapboxCommon/Turf
  # come along transitively via CocoaPods' pod-name deduplication).
  #
  # WARNING: REVERTED from vendoring MapboxMaps.xcframework directly (from
  # mapbox-maps-ios-binary) + a Podfile-level override forcing
  # @rnmapbox/maps to use that copy. That approach targeted a real,
  # confirmed root cause (mapbox/mapbox-maps-ios#1669: CocoaPods-trunk
  # MapboxMaps lacks BUILD_LIBRARY_FOR_DISTRIBUTION, causing a DYLD
  # missing-symbol crash) - but the override itself broke CocoaPods'
  # automatic `[CP] Copy XCFrameworks` build phase generation for that
  # specific pod (confirmed via a real build log: the phase ran for every
  # sibling Mapbox pod except the overridden one), causing a persistent
  # "no such module 'MapboxMaps'" compile error that several rounds of
  # manual xcconfig patching couldn't fully resolve.
  #
  # WARNING: HONEST CAVEAT: the reference implementation's own
  # verification was about the BUILD succeeding, not about confirmed
  # crash-free behavior on a real device. The original DYLD "Symbol not
  # found: GestureType.singleTap" launch crash this whole investigation
  # started from was never confirmed fixed by this specific change, and
  # this reversion could plausibly reintroduce it. This reversion's goal
  # is narrower: get past the currently-blocking compile-time error
  # first.
  #
  # DYNAMIC as of 5.0.5: resolved at the top of this file from the
  # consumer's `iosMapboxMapsVersion` plugin option (via
  # ios/.mapbox-ios-versions.json, written at prebuild), defaulting to
  # 11.20.2 - the confirmed-working interlocked set
  # (nav 3.20.1 / MapboxNavigationNative 324.20.2 / MapboxCommon 24.20.2
  # / MapboxMaps 11.20.2). WARNING: whoever overrides it is responsible
  # for (a) matching the consuming app's `RNMapboxMapsVersion` exactly
  # (a mismatch fails `pod install` loudly with a version conflict - by
  # design), (b) pairing it correctly with `iosMapboxNavigationVersion`
  # (a bad pair means link errors or runtime ABI crashes), and
  # (c) checking whether the chosen versions raise the minimum iOS
  # deployment target above s.platforms up top.
  s.dependency 'MapboxMaps', mapbox_maps_version

  # Matches stefanpavlovic-tech/react-native-mapbox-navigation's own
  # podspec (the working reference implementation). An earlier version of
  # this podspec removed this setting based on a theory (a static/dynamic
  # linkage mismatch) that later turned out to be wrong - it was never
  # restored afterward, an oversight rather than a deliberate choice.
  s.static_framework = true

  # -- iOS: Mapbox Navigation SDK v3, via VENDORED XCFRAMEWORKS ---------------
  #
  # Mapbox Navigation SDK v3 is distributed as SOURCE CODE via SPM only - it
  # has no CocoaPods support. So: ios/Frameworks/*.xcframework are fetched
  # ONCE (see ios/fetch-xcframeworks.sh - downloads official precompiled
  # binaries from mapbox-navigation-ios-build-artifacts) and committed to
  # this package.
  #
  # IMPORTANT - do NOT vendor MapboxMaps/MapboxCommon/MapboxCoreMaps/Turf
  # here. @rnmapbox/maps already installs those via CocoaPods, and
  # MapboxNavigationCore.xcframework is built to link against that SAME
  # version (kept in sync - see ios/fetch-xcframeworks.sh for the
  # version-alignment requirement). Vendoring a second copy of those
  # specific frameworks would reintroduce duplicate-symbol errors, per the
  # exact guidance a Mapbox engineer gave for this same scenario on
  # mapbox/mapbox-navigation-ios#4703.
  # FIXED: this was previously a Ruby `Dir.glob(...)` expression, which
  # executes AT PODSPEC PARSE TIME - i.e. during CocoaPods' dependency
  # analysis, BEFORE prepare_command (or any other fetch mechanism) has
  # downloaded anything into ios/Frameworks/. Since these frameworks are
  # deliberately never committed (see prepare_command below), that glob
  # deterministically returned an empty array on every fresh install,
  # freezing `s.vendored_frameworks = []` into the parsed spec - so even
  # when the fetch itself succeeded afterward, nothing was ever linked.
  # `pod install` does not treat an empty vendored_frameworks list as an
  # error, which is why this passed silently and only surfaced later as
  # "no such module 'MapboxNavigationCore'" at Xcode compile time.
  #
  # Fix: a plain glob PATTERN STRING instead of a pre-resolved Ruby array.
  # CocoaPods natively supports wildcards in file-pattern attributes and
  # resolves them later in its own install flow, after pod sources are
  # downloaded - so frameworks fetched by prepare_command (or by the
  # Podfile-level fetch, if configured - see plugin/src/index.js) are
  # actually visible when this pattern gets expanded.
  s.vendored_frameworks = 'ios/Frameworks/*.xcframework'

  # WARNING: CHANGED from a recursive glob (`ios/**/*.{swift,h,m,mm}`) to a
  # non-recursive one, matching a real, confirmed-working reference
  # implementation's own fix for a real bug: a recursive source_files
  # glob COMBINED WITH an exclude_files pattern targeting
  # ios/Frameworks/*.xcframework/**/*.h can cause CocoaPods to strip the
  # vendored frameworks themselves too (it applies exclude_files broadly),
  # producing a "no such module" error for a vendored framework -
  # confirmed by stefanpavlovic-tech/react-native-mapbox-navigation's own
  # commit 6ede7e1 fixing exactly this. This project's own Swift/ObjC
  # source all lives directly under ios/ (not nested in subdirectories),
  # so a non-recursive glob covers everything needed without ever
  # descending into ios/Frameworks/ at all - removing the need for
  # exclude_files entirely.
  s.source_files = 'ios/*.{swift,h,m,mm}'

  s.pod_target_xcconfig = {
    'DEFINES_MODULE'             => 'YES',
    'SWIFT_COMPILATION_MODE'     => 'wholemodule',
    # s.platforms alone was tried as the single source of truth for the
    # deployment target, on the theory that duplicating it here risked the
    # two declarations going out of sync. That theory was wrong -
    # confirmed by a real build still failing with "compiling for iOS
    # 14.0" even with s.platforms set to 15.1 and this line removed.
    # s.platforms governs CocoaPods' dependency-compatibility validation;
    # it does NOT reliably force the actual IPHONEOS_DEPLOYMENT_TARGET
    # build setting used to invoke the compiler for this target in this
    # project - something (likely the generated Podfile's own default
    # platform declaration) overrides it back down to a lower value
    # otherwise. Both declarations are needed; when bumping the vendored
    # SDK version, update BOTH this value and s.platforms above.
    'IPHONEOS_DEPLOYMENT_TARGET' => '15.1',

    # -- Swift-toolchain-mismatch fix (Xcode 26.2 / Expo SDK 55) -------------
    # Root-caused from a real EAS build log, then confirmed verbatim in the
    # Swift compiler's own source on the exact failing toolchain's branch
    # (swiftlang/swift, release/6.2, lib/Frontend/ModuleInterfaceLoader.cpp).
    #
    # SYMPTOM: compiling this pod's ExpoMapboxNavigationView.swift failed with
    #   error: failed to build module 'MapboxNavigationCore'; this SDK is not
    #   supported by the compiler (the SDK is built with 'Apple Swift version
    #   6.1.2', while this compiler is 'Apple Swift version 6.2.3')
    # preceded by, from inside MapboxNavigationCore's own .swiftinterface:
    #   error: cannot load underlying module for 'MapboxMaps'
    #
    # WHY IT HAPPENS: our vendored MapboxNavigationCore/UIKit xcframeworks are
    # precompiled by Mapbox with a specific Swift (6.1.2 for nav 3.20.1, i.e.
    # Xcode 16.4). A binary .swiftmodule is only loadable by the exact same
    # compiler version, so a newer Xcode legitimately rejects it. That alone
    # is NOT fatal: these frameworks ship with BUILD_LIBRARY_FOR_DISTRIBUTION,
    # so Swift's designed fallback is to REBUILD the module from its textual
    # .swiftinterface with the current compiler. That fallback is what
    # actually failed here - and only because of a context-propagation
    # detail, not because of anything version-specific.
    #
    # THE MECHANISM (ModuleInterfaceLoader.cpp, release/6.2 lines ~1950-1973):
    # the interface rebuild runs in a sub-invocation, and that sub-invocation
    # inherits the parent compile's `-Xcc` flags ONLY when one of
    # `strictImplicitModuleContext` / `disableImplicitSwiftModule` /
    # `DirectClangCC1ModuleBuild` is set; otherwise the inherited clang-arg
    # list is deliberately reduced (search-path flags dropped, to keep module
    # sharing viable across targets). MapboxMaps is built FROM SOURCE by
    # CocoaPods, so the only way to reach its Clang module is the explicit
    # `-Xcc -fmodule-map-file=<build-products>/MapboxMaps/MapboxMaps.modulemap`
    # that Xcode passes to THIS target (confirmed present in the failing
    # build log). Dropped in the sub-invocation, `import MapboxMaps` inside
    # MapboxNavigationCore's interface becomes unresolvable -> "cannot load
    # underlying module" -> the whole rebuild fails -> the compiler reports
    # the original version mismatch as the top-level error. (Explicit modules
    # are OFF in this build - verified in the log - so none of the other
    # three conditions applied.)
    #
    # THE FIX: ask for that strict forwarding explicitly. The interface
    # rebuild then sees the same modulemap flags as this target, resolves
    # MapboxMaps, and MapboxNavigationCore/UIKit rebuild cleanly against the
    # current compiler. This fixes the CLASS of problem rather than one
    # instance of it: no vendored-SDK/Xcode version pairing is required
    # anymore, which is exactly what library evolution is supposed to buy.
    # NOTE (5.1.7): this pod target is NOT the only one that has to rebuild
    # these interfaces - the app target does too (see user_target_xcconfig
    # below), so the same flag is repeated there.
    'OTHER_SWIFT_FLAGS' => '$(inherited) -Xfrontend -strict-implicit-module-context',
  }

  # -- Avoid the .private.swiftinterface toolchain-version check -------------
  # Debug builds default to ENABLE_TESTABILITY = YES (needed for @testable
  # import elsewhere in the project). That setting makes Xcode re-verify our
  # vendored frameworks' .private.swiftinterface (the "testable" textual
  # interface) instead of just linking the precompiled .swiftmodule binary
  # directly - and THAT re-verification step is what triggers Swift's
  # strict "this SDK is not supported by the compiler" check if the Swift
  # compiler that built the vendored xcframeworks differs at all from the
  # one doing the build. Precompiled binaries built with library evolution
  # enabled (which is how Mapbox ships these) are meant to tolerate a newer
  # consuming compiler without this stricter recheck - disabling
  # testability avoids forcing that recheck in the first place. This is
  # scoped to app (not test) targets; if your project relies on @testable
  # import of your OWN code elsewhere, this setting does not affect that
  # - it only affects whether Xcode treats imports of vendored/third-party
  # frameworks like this one as needing their private interface.
  #
  # -- App target: same interface-rebuild context fix as the pod target -----
  # (5.1.7) With 5.1.6 the ExpoMapboxNavigation pod compiled fine on Xcode
  # 26.2, but the build then failed in the APP target ('Navio'): its
  # ExpoModulesProvider.swift does `import ExpoMapboxNavigation`, and loading
  # that module transitively loads MapboxNavigationCore/UIKit - whose binary
  # .swiftmodule (Swift 6.3.2) is again rejected, so the app target runs the
  # SAME .swiftinterface rebuild, with the SAME dropped `-Xcc
  # -fmodule-map-file=...MapboxMaps.modulemap` (CocoaPods passes those to the
  # aggregate Pods-<App> xcconfig too), and fails with the same "cannot load
  # underlying module for 'MapboxMaps'". OTHER_SWIFT_FLAGS is a CocoaPods
  # PLURAL setting, so this value is merged with other pods' flags (not
  # overwritten), and the Expo app template does not set OTHER_SWIFT_FLAGS
  # itself, so the app target inherits it from Pods-<App>.xcconfig.
  s.user_target_xcconfig = {
    'ENABLE_TESTABILITY' => 'NO',
    'OTHER_SWIFT_FLAGS' => '$(inherited) -Xfrontend -strict-implicit-module-context',
  }

  # WARNING: CHANGED: this now actually FETCHES the required xcframeworks (via
  # ios/fetch-xcframeworks.sh) instead of just warning that they're
  # missing. ios/Frameworks/*.xcframework is no longer committed to this
  # package's repo or npm tarball (see .gitignore and package.json's
  # "files" - both updated together with this change) - Mapbox's own
  # Product Terms ("1.10. No Redistribution") prohibit redistributing
  # their SDK binaries to third parties who haven't authenticated with
  # their own Mapbox account/token, and this repository is public.
  # Instead, whichever app actually consumes this package fetches these
  # binaries itself, using ITS OWN Mapbox DOWNLOADS:READ token, right here
  # at `pod install` time - matching the same approach used by other
  # public Mapbox Navigation + React Native packages (e.g.
  # pawan-pk/react-native-mapbox-navigation).
  #
  # Requires: network access, and a valid ~/.netrc with Mapbox
  # DOWNLOADS:READ credentials - written automatically by this package's
  # own Expo config plugin from its `downloadsToken` option (see
  # plugin/src/index.js), which runs during `expo prebuild`, always
  # before `pod install`.
  # CHANGED from a conditional check (only fetch if ios/Frameworks looks
  # empty) to an unconditional fetch every time. The conditional's own
  # bash logic was verified correct (tested directly: `[ -z "$(ls -A
  # ios/Frameworks 2>/dev/null)" ]` evaluates true for a directory that
  # doesn't exist at all, which is this package's actual real-world state
  # - ios/Frameworks isn't committed at all, not even as an empty
  # directory, per .gitignore), so this wasn't a confirmed bug - but a
  # real `pod install` log showed zero visible output from this command
  # either way (nothing between "Installing ExpoMapboxNavigation" and the
  # next pod), leaving genuine uncertainty about whether it ran at all.
  # Removing the conditional removes one possible point of failure
  # outright rather than trying to prove a negative from an ambiguous
  # log. `ios/fetch-xcframeworks.sh` itself is safe to run repeatedly -
  # `rm -rf` before each `cp -R` inside it - so this doesn't risk leaving
  # stale mixed content behind on a second run.
  s.prepare_command = <<-CMD
    echo ""
    echo "================================================================"
    echo "[ExpoMapboxNavigation] prepare_command starting - fetching Mapbox"
    echo "Navigation xcframeworks now (this can take several minutes)..."
    echo "================================================================"
    if [ ! -f "ios/fetch-xcframeworks.sh" ]; then
      echo "error: [ExpoMapboxNavigation] ios/fetch-xcframeworks.sh not found - cannot fetch required binaries."
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
    echo "[ExpoMapboxNavigation] prepare_command finished successfully."
  CMD
end
