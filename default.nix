{ pkgs ? (import ./nix/pinned-nixpkgs.nix {}) }:
let
  inherit (pkgs) newScope;
  inherit (pkgs.lib) makeScope;
in

makeScope newScope (self: with self; {
  # To update the pinned Zephyr dependecies using west and update-manifest:
  #  nix shell -f . west -c west init -l app
  #  nix shell -f . west -c west update
  #  nix shell -f . update-manifest -c update-manifest > nix/manifest.json
  # Note that any `group-filter` groups in west.yml need to be temporarily
  # removed, as `west update-manifest` requires all dependencies to be fetched.
  update-manifest = callPackage ./nix/update-manifest { };

  west = pkgs.python3Packages.west;

  combine_uf2 = a: b: pkgs.runCommandNoCC "combined_${a.name}_${b.name}" {}
  ''
    mkdir -p $out
    cat ${a}/zmk.uf2 ${b}/zmk.uf2 > $out/glove80.uf2
    [[ -e ${a}/.config ]] && cp ${a}/.config $out/${a.name}.config
    [[ -e ${a}/zephyr.dts ]] && cp ${a}/zephyr.dts $out/${a.name}.zephyr.dts
    [[ -e ${a}/zephyr.dts.pre ]] && cp ${a}/zephyr.dts.pre $out/${a.name}.zephyr.dts.pre
    [[ -e ${b}/.config ]] && cp ${b}/.config $out/${b.name}.config
    [[ -e ${b}/zephyr.dts ]] && cp ${b}/zephyr.dts $out/${b.name}.zephyr.dts
    [[ -e ${b}/zephyr.dts.pre ]] && cp ${b}/zephyr.dts.pre $out/${b.name}.zephyr.dts.pre
  '';

  zephyr = callPackage ./nix/zephyr.nix { };

  zmk = callPackage ./nix/zmk.nix { };

  zmk_settings_reset = zmk.override {
    shield = "settings_reset";
  };

  glove80_left = zmk.override {
    board = "glove80_lh";
  };

  glove80_right = zmk.override {
    board = "glove80_rh";
  };

  glove80_combined = combine_uf2 glove80_left glove80_right;

  glove80_v0_left = zmk.override {
    board = "glove80_v0_lh";
  };

  glove80_v0_right = zmk.override {
    board = "glove80_v0_rh";
  };
})
