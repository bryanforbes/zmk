/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <sys/types.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/battery.h>
#include <zmk/events/battery_state_changed.h>

static uint8_t last_state_of_peripheral_charge = 0;

uint8_t zmk_battery_state_of_peripheral_charge() { return last_state_of_peripheral_charge; }

int peripheral_batt_lvl_listener(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *ev =
        as_zmk_peripheral_battery_state_changed(eh);
    if (ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    };
    LOG_DBG("Peripheral battery level event: %u", ev->state_of_charge);
    last_state_of_peripheral_charge = ev->state_of_charge;

    return 0;
};

ZMK_LISTENER(peripheral_batt_lvl_listener, peripheral_batt_lvl_listener);
ZMK_SUBSCRIPTION(peripheral_batt_lvl_listener, zmk_peripheral_battery_state_changed);
