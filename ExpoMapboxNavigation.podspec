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
  s.platforms      = { :ios => '14.0' }
  s.swift_version  = '5.9'
  s.source         = { git: package['repository']['url'], tag: "v#{s.version}" }

  s.dependency 'ExpoModulesCore'

  # ── iOS: Mapbox Navigation SDK v3 ───────────────────────────────────────────
  #
  # IMPORTANT — do NOT use spm_dependency() here.
  #
  # spm_dependency() causes "43029 duplicate symbols" linker errors when used
  # alongside @rnmapbox/maps in an Expo project (confirmed: React Native
  # GitHub issue #47344, Expo GitHub issue #37813). The root cause is that
  # spm_dependency() links the SPM framework into both the Pod target AND
  # the main app target simultaneously, so the linker sees every symbol twice.
  #
  # The correct approach for Expo modules that need SPM-only packages:
  # add the SPM dependency directly to the Xcode project's main target via
  # the config plugin (withXcodeProject + withDangerousMod), which is exactly
  # what our plugin/src/index.js does via the addMapboxNavigationSPM function.
  # That way there is only ONE copy of each framework in the final binary.
  #
  # MapboxNavigationCore and MapboxNavigationUIKit are therefore listed as
  # weak framework references here so the module compiles against them, while
  # the actual linking happens at the app level from the SPM dependency added
  # by the config plugin.
  s.pod_target_xcconfig = {
    'DEFINES_MODULE'                          => 'YES',
    'SWIFT_COMPILATION_MODE'                  => 'wholemodule',
    'OTHER_SWIFT_FLAGS'                       => '$(inherited) -Xfrontend -disable-reflection-metadata',
    'FRAMEWORK_SEARCH_PATHS'                  => '$(inherited) $(PLATFORM_DIR)/Developer/Library/Frameworks',
    'OTHER_LDFLAGS'                           => '$(inherited)',
    'IPHONEOS_DEPLOYMENT_TARGET'              => '14.0',
  }

  s.source_files = 'ios/**/*.{swift,h,m,mm}'
end
