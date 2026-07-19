// This file is Expo's conventional config-plugin entry point (loaded from
// the package root before anything under plugin/) — do NOT duplicate the
// plugin implementation here. A prior version of this file WAS a hand
// copy-pasted snapshot of plugin/src/index.js, and every edit made only to
// plugin/src/index.js afterward (iOS dynamic SDK version selection in
// 5.0.5, the Android Maven credentials fix in 5.0.7, and everything in
// between) silently never reached consumers, because Expo always loads
// THIS file, never plugin/src/index.js directly — confirmed the hard way
// via a real EAS build still failing with a bug fixed two versions
// earlier. Keep this a thin proxy, permanently.
module.exports = require('./plugin/src/index.js');
