/*
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_caps_word

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/modifiers_state_changed.h>
#include <zmk/keys.h>
#include <zmk/hid.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct key_list {
    const struct zmk_key_param *keys;
    size_t len;
};

struct behavior_caps_word_config {
    struct key_list continuations;
    struct key_list shifted;
    int idle_timeout_ms;
    zmk_mod_flags_t mods;
    bool no_default_keys;
};

struct behavior_caps_word_data {
    struct k_work_delayable idle_timer;
    bool active;
};

static const struct device *devs[DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT)];
static size_t dev_count = 0;
static int pressed_key_count = 0;

static void restart_caps_word_idle_timer(const struct device *dev) {
    const struct behavior_caps_word_config *config = dev->config;
    struct behavior_caps_word_data *data = dev->data;

    if (config->idle_timeout_ms) {
        k_work_schedule(&data->idle_timer, K_MSEC(config->idle_timeout_ms));
    }
}

static void restart_caps_word_idle_timer_all_devs(void) {
    for (int i = 0; i < dev_count; i++) {
        if (devs[i]) {
            restart_caps_word_idle_timer(devs[i]);
        }
    }
}

static void cancel_caps_word_idle_timer(const struct device *dev) {
    const struct behavior_caps_word_config *config = dev->config;
    struct behavior_caps_word_data *data = dev->data;

    if (config->idle_timeout_ms) {
        k_work_cancel_delayable(&data->idle_timer);
    }
}

static void activate_caps_word(const struct device *dev) {
    struct behavior_caps_word_data *data = dev->data;

    data->active = true;
    restart_caps_word_idle_timer(dev);
}

static void deactivate_caps_word(const struct device *dev) {
    struct behavior_caps_word_data *data = dev->data;

    data->active = false;
    cancel_caps_word_idle_timer(dev);
}

static int on_caps_word_binding_pressed(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct behavior_caps_word_data *data = dev->data;

    if (data->active) {
        deactivate_caps_word(dev);
    } else {
        activate_caps_word(dev);
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_caps_word_binding_released(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_caps_word_driver_api = {
    .binding_pressed = on_caps_word_binding_pressed,
    .binding_released = on_caps_word_binding_released,
};

static int caps_word_keycode_state_changed_listener(const zmk_event_t *eh);

ZMK_LISTENER(behavior_caps_word, caps_word_keycode_state_changed_listener);
ZMK_SUBSCRIPTION(behavior_caps_word, zmk_keycode_state_changed);

static bool key_list_contains(const struct key_list *list, uint16_t usage_page, zmk_key_t usage_id,
                              zmk_mod_flags_t modifiers) {
    for (int i = 0; i < list->len; i++) {
        const struct zmk_key_param *key = &list->keys[i];

        if (key->page == usage_page && key->id == usage_id &&
            (key->modifiers & modifiers) == key->modifiers) {
            return true;
        }
    }

    return false;
}

static bool caps_word_is_alpha(uint16_t usage_page, zmk_key_t usage_id) {
    return (usage_page == HID_USAGE_KEY && usage_id >= HID_USAGE_KEY_KEYBOARD_A &&
            usage_id <= HID_USAGE_KEY_KEYBOARD_Z);
}

static bool caps_word_is_numeric(uint16_t usage_page, zmk_key_t usage_id) {
    return (usage_page == HID_USAGE_KEY && usage_id >= HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION &&
            usage_id <= HID_USAGE_KEY_KEYBOARD_0_AND_RIGHT_PARENTHESIS);
}

static bool caps_word_should_enhance(const struct behavior_caps_word_config *config,
                                     struct zmk_keycode_state_changed *ev) {
    if (!config->no_default_keys && caps_word_is_alpha(ev->usage_page, ev->keycode)) {
        return true;
    }

    zmk_mod_flags_t modifiers = ev->implicit_modifiers | zmk_hid_get_explicit_mods();

    return key_list_contains(&config->shifted, ev->usage_page, ev->keycode, modifiers);
}

static bool caps_word_is_continuation(const struct behavior_caps_word_config *config,
                                      const struct zmk_keycode_state_changed *ev) {
    if (is_mod(ev->usage_page, ev->keycode) || caps_word_should_enhance(config, ev)) {
        return true;
    }

    if (!config->no_default_keys && caps_word_is_numeric(ev->usage_page, ev->keycode)) {
        return true;
    }

    zmk_mod_flags_t modifiers = ev->implicit_modifiers | zmk_hid_get_explicit_mods();

    return key_list_contains(&config->continuations, ev->usage_page, ev->keycode, modifiers);
}

static int caps_word_keycode_state_changed_listener(const zmk_event_t *eh) {
    struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    if (ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (!ev->state) {
        // Idle timer should only run when all keys are released.
        pressed_key_count--;
        pressed_key_count = MAX(pressed_key_count, 0);

        if (pressed_key_count == 0) {
            restart_caps_word_idle_timer_all_devs();
        }

        return ZMK_EV_EVENT_BUBBLE;
    }

    pressed_key_count++;

    for (int i = 0; i < dev_count; i++) {
        const struct device *dev = devs[i];
        if (dev == NULL) {
            continue;
        }

        struct behavior_caps_word_data *data = dev->data;
        if (!data->active) {
            continue;
        }

        cancel_caps_word_idle_timer(dev);

        const struct behavior_caps_word_config *config = dev->config;

        if (caps_word_should_enhance(config, ev)) {
            LOG_DBG("Enhancing usage 0x%02X with modifiers: 0x%02X", ev->keycode, config->mods);
            ev->implicit_modifiers |= config->mods;
        }

        if (!caps_word_is_continuation(config, ev)) {
            LOG_DBG("Deactivating caps_word for 0x%02X - 0x%02X", ev->usage_page, ev->keycode);
            deactivate_caps_word(dev);
        }
    }

    return ZMK_EV_EVENT_BUBBLE;
}

static void caps_word_timeout_handler(struct k_work *work) {
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);
    struct behavior_caps_word_data *data =
        CONTAINER_OF(dwork, struct behavior_caps_word_data, idle_timer);

    LOG_DBG("Deactivating caps_word for idle timeout");
    data->active = false;
}

static int behavior_caps_word_init(const struct device *dev) {
    const struct behavior_caps_word_config *config = dev->config;
    struct behavior_caps_word_data *data = dev->data;

    if (config->idle_timeout_ms) {
        k_work_init_delayable(&data->idle_timer, caps_word_timeout_handler);
    }

    __ASSERT(dev_count < ARRAY_SIZE(devs), "Too many devices");

    devs[dev_count] = dev;
    dev_count++;
    return 0;
}

#define KEY_ARRAY_ITEM(i, n, prop) ZMK_KEY_DECODE_PARAM(DT_INST_PROP_BY_IDX(n, prop, i))

#define KEY_LIST_PROP(n, prop)                                                                     \
    COND_CODE_1(DT_NODE_HAS_PROP(DT_DRV_INST(n), prop),                                            \
                ({LISTIFY(DT_INST_PROP_LEN(n, prop), KEY_ARRAY_ITEM, (, ), n, prop)}), ({}))

#define KEY_LIST(array) ((struct key_list){.keys = array, .len = ARRAY_SIZE(array)})

#define EMPTY_KEY_LIST ((struct key_list){.keys = NULL, .len = 0})

#define KP_INST(n)                                                                                 \
    static const struct zmk_key_param caps_word_continue_list_##n[] =                              \
        KEY_LIST_PROP(n, continue_list);                                                           \
                                                                                                   \
    static const struct zmk_key_param caps_word_shift_list_##n[] = KEY_LIST_PROP(n, shift_list);   \
                                                                                                   \
    static struct behavior_caps_word_data behavior_caps_word_data_##n = {.active = false};         \
                                                                                                   \
    static const struct behavior_caps_word_config behavior_caps_word_config_##n = {                \
        .mods = DT_INST_PROP_OR(n, mods, MOD_LSFT),                                                \
        .idle_timeout_ms = DT_INST_PROP(n, idle_timeout_ms),                                       \
        .no_default_keys = DT_INST_PROP(n, no_default_keys),                                       \
        .continuations = KEY_LIST(caps_word_continue_list_##n),                                    \
        .shifted = KEY_LIST(caps_word_shift_list_##n),                                             \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_caps_word_init, NULL, &behavior_caps_word_data_##n,        \
                            &behavior_caps_word_config_##n, APPLICATION,                           \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_caps_word_driver_api);

DT_INST_FOREACH_STATUS_OKAY(KP_INST)

#endif
