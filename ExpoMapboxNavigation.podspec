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

  # ── iOS: Mapbox Navigation SDK v3 via SPM ─────────────────────────────────
  #
  # Mapbox Navigation SDK v3 is SPM-only (CocoaPods "coming soon" per Mapbox).
  # We do NOT use spm_dependency() here — that causes 43029 duplicate symbol
  # linker errors when used alongside @rnmapbox/maps.
  #
  # Instead, our config plugin (app.plugin.js) injects a post_install hook
  # into the Podfile. The hook uses the Xcodeproj Ruby API — the same technique
  # as @rnmapbox/maps itself — to add mapbox-navigation-ios as a SPM
  # dependency to the ExpoMapboxNavigation pod target and the app target.
  # find-or-create semantics prevent any duplication.
  #
  # The post_install hook runs during `pod install`, with full access to
  # installer.pods_project and installer.aggregate_targets, which is the only
  # correct way to add SPM packages alongside CocoaPods in Expo projects.

  s.source_files = 'ios/**/*.{swift,h,m,mm}'
  s.exclude_files = [
    'ios/Package.swift',
    'ios/build-xcframeworks.sh',
  ]

  s.pod_target_xcconfig = {
    'DEFINES_MODULE'             => 'YES',
    'SWIFT_COMPILATION_MODE'     => 'wholemodule',
    'IPHONEOS_DEPLOYMENT_TARGET' => '14.0',
  }
end
