# Glove80 ZMK Firmware: Personal fork

This is my personal Glove80 ZMK fork containing various experimental features. I try to rebase it regularly
onto Moergo's latest upstream. Below is the list of features currently included in the `bryanforbes` branch
_on top of_ Moergo's ZMK fork:

- **Zephyr 3.2** (PR [#19](https://github.com/moergo-sc/zmk/pull/19)) - official Moergo PR to update their fork to Zephyr 3.2
- **mouse** (PR [#778](https://github.com/zmkfirmware/zmk/pull/778)) - official PR + ftc's changes + [fixes needed for Zephyr 3.2](https://github.com/urob/zmk/tree/mouse-3.2) + safeguads + always use hog device + modifications for Glove80
- **swapper** (PR [#1366](https://github.com/zmkfirmware/zmk/pull/1366)) - official PR
- **global-quick-tap-ms** (PR [#1387](https://github.com/zmkfirmware/zmk/pull/1387)) - official PR
- **caps-word** (PR [#1742](https://github.com/zmkfirmware/zmk/pull/1742) - official PR
- **on-release-for-tap-preferred** - [on-release feature for tap-preferred](https://github.com/celejewski/zmk/commit/d7a8482712d87963e59b74238667346221199293) by Andrezej (cherry-picked from [urob's](https://github.com/urob/zmk) fork)

See my [Glove80 ZMK config](https://github.com/bryanforbes/glove80-zmk-config/) repo to see how I build firmware images for the Glove80.
