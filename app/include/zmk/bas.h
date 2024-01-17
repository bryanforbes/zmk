/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

struct zmk_bas_battery_level_status {
    uint8_t flags;
    uint16_t power_state;
    uint16_t identifier;
    uint8_t battery_level;
};

enum zmk_bas_charge_state {
    ZMK_BAS_CHARGE_UNKNOWN = 0,
    ZMK_BAS_CHARGE_CHARGING = 1,
    ZMK_BAS_CHARGE_DISCHARGING_ACTIVE = 2,
    ZMK_BAS_CHARGE_DISCHARGING_INACTIVE = 3,
};

uint8_t zmk_bas_get_battery_level(void);
int zmk_bas_set_battery_level(uint8_t level);
int zmk_bas_set_charge_state(enum zmk_bas_charge_state charge_state);
