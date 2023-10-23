/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/keys.h>

struct zmk_keycode_state_changed {
    int64_t timestamp;
    zmk_key_t keycode;
    uint16_t usage_page;
    zmk_mod_flags_t implicit_modifiers;
    zmk_mod_flags_t explicit_modifiers;
    bool state;
};

ZMK_EVENT_DECLARE(zmk_keycode_state_changed);

static inline struct zmk_keycode_state_changed_event *
zmk_keycode_state_changed_from_encoded(uint32_t encoded, bool pressed, int64_t timestamp) {
    const struct zmk_key_param key = ZMK_KEY_DECODE_PARAM(encoded);
    const bool is_explicit = is_mod(key.page, key.id);

    return new_zmk_keycode_state_changed(
        (struct zmk_keycode_state_changed){.usage_page = key.page,
                                           .keycode = key.id,
                                           .implicit_modifiers = is_explicit ? 0 : key.modifiers,
                                           .explicit_modifiers = is_explicit ? key.modifiers : 0,
                                           .state = pressed,
                                           .timestamp = timestamp});
}
