# Glove80 ZMK Firmware: Personal fork

This is my personal Glove80 ZMK fork containing various experimental features. I try to rebase it regularly
onto Moergo's latest upstream. Below is the list of features currently included in the `bryanforbes` branch
_on top of_ Moergo's ZMK fork:

- **Moergo's 24.3-alpha branch** - official Moergo fork of ZMK
- **nix improvements** - My own tweaks to the nix build process
- **pointer movement/scrolling** (PR [#2027](https://github.com/zmkfirmware/zmk/pull/2027)) - official PR
- **swapper** (PR [#1366](https://github.com/zmkfirmware/zmk/pull/1366)) - official PR
- **caps-word** (PR [#1742](https://github.com/zmkfirmware/zmk/pull/1742) - official PR
- **on-release-for-tap-preferred** - [on-release feature for tap-preferred](https://github.com/celejewski/zmk/commit/d7a8482712d87963e59b74238667346221199293) by Andrezej (cherry-picked from [urob's](https://github.com/urob/zmk) fork)
- **better battery level reporting** (PR [#2066](https://github.com/zmkfirmware/zmk/pull/2066) - official PR

See my [Glove80 ZMK config](https://github.com/bryanforbes/glove80-zmk-config/) repo to see how I build firmware images for the Glove80.
