---
title: Caps Word Behavior
sidebar_label: Caps Word
---

## Summary

The caps word behavior behaves similar to caps lock, but it will automatically deactivate at the end of a word. This is useful for typing single words in all capitals, such as abbreviations or identifiers in code. This is especially useful for smaller keyboards using [mod-taps](/docs/behaviors/mod-tap) for shift, where it can help avoid repeated alternating holds when typing words in all caps.

When caps word is active, shift is added to capitalize letters. Caps word deactivates at the end of a word, that is when any key is pressed other than alphanumeric characters, `UNDERSCORE`, `BACKSPACE`, or `DELETE`. It also deactivates if the caps word key is pressed again, or when the keyboard is idle for 5 seconds.

### Behavior Binding

- Reference: `&caps_word`

Example:

```
&caps_word
```

### Configuration

#### Shift List

By default, caps word will apply the shift modifier to only alpha keys. If you would like additional keys to be shifted, you can set an array of keys in the `shift-list` property in your keymap. Any keys added to this list will both continue the word and be shifted.

For example, to make caps word change `-` to `_` for typing constants in code such as `GPIO_ACTIVE_LOW`, add `MINUS` to the `shift-list`:

```
&caps_word {
    shift-list = <MINUS>;
};

/ {
    keymap {
        ...
    };
};
```

Alpha keys are automatically included in the list. This can be disabled by adding a [`no-default-keys`](#non-us-layouts) property.

#### Continue List

By default, caps word will remain active when any alphanumeric key, modifier key, key listed in `shift-list`, or `UNDERSCORE`, `BACKSPACE`, or `DELETE` is pressed. Any other key will turn off caps word. If you would like to override this, you can set a new array of keys in the `continue-list` property in your keymap. Any keys added to this list will continue a word but not be shifted.

For example, to add left/right arrow keys to the default list, add:

```
&caps_word {
    continue-list = <UNDERSCORE BACKSPACE DELETE LEFT RIGHT>;
};

/ {
    keymap {
        ...
    };
};
```

Alphanumeric keys are automatically included in the list. This can be disabled by adding a [`no-default-keys`](#non-us-layouts) property.

#### Applied Modifier(s)

In addition, if you would like caps word to apply different or _multiple_ modifiers instead of just `MOD_LSFT`, you can override the `mods` property:

```
&caps_word {
    mods = <(MOD_LSFT | MOD_LALT)>;
};

/ {
    keymap {
        ...
    };
};
```

#### Idle Timeout

By default, caps word turns off automatically if no keys are pressed for 5 seconds. This can be changed by setting the `idle-timeout-ms` property in your keymap. This value is in milliseconds.

For example, this would change the timeout to 10 seconds:

```
&caps_word {
    idle-timeout-ms = <10000>;
};
```

Setting the timeout to 0 configures caps word to never time out. It will remain active until you press a key that turns off caps word.

### Non-US Layouts

Alphanumeric keys (A-Z, 0-9) are automatically included in `continue-list`, and alpha keys (A-Z) are automatically included in `shift-list`. This may result in unexpected behaviors for some OS keyboard layouts, for example in Dvorak where the quote key sends the Q keycode, and therefore is treated as continuing a word. You can disable this and manually specify the full lists by adding a `no-default-keys` property:

```
// Keycodes for Dvorak layout
#define DV_A A
#define DV_B N
...

&caps_word {
    no-default-keys;
    continue-list = <N0 N1 N2 N3 N4 N5 N6 N7 N8 N9 DV_UNDER BACKSPACE DELETE>;
    shift-list = <DV_A DV_B DV_C ... DV_Z DV_MINUS>;
};
```

### Multiple Caps Breaks

If you want to use multiple caps breaks with different codes to break the caps, you can add additional caps words instances to use in your keymap:

```
/ {
    prog_caps: behavior_prog_caps_word {
        compatible = "zmk,behavior-caps-word";
        label = "PROG_CAPS";
        #binding-cells = <0>;
        continue-list = <UNDERSCORE>;
    };

    keymap {
        ...
    };
};
```
