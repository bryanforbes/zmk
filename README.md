# Glove80 ZMK Firmware: Personal fork

This is my personal Glove80 ZMK fork containing various experimental features. I try to rebase it regularly
onto Moergo's latest upstream. Below is the list of features currently included in the `bryanforbes` branch
_on top of_ Moergo's ZMK fork:

- **Moergo's PR26 branch** - official Moergo fork of ZMK
- **nix improvements** - My own tweaks to the nix build process
- **mouse** Commits from [caksoylar's branch](https://github.com/caksoylar/zmk/tree/feat/mouse-keys-3.2) - unofficial
- **swapper** (PR [#1366](https://github.com/zmkfirmware/zmk/pull/1366)) - official PR
- **caps-word** (PR [#1742](https://github.com/zmkfirmware/zmk/pull/1742) - official PR
- **on-release-for-tap-preferred** - [on-release feature for tap-preferred](https://github.com/celejewski/zmk/commit/d7a8482712d87963e59b74238667346221199293) by Andrezej (cherry-picked from [urob's](https://github.com/urob/zmk) fork)

See my [Glove80 ZMK config](https://github.com/bryanforbes/glove80-zmk-config/) repo to see how I build firmware images for the Glove80.
