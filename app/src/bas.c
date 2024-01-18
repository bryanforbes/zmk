/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>
#include <zephyr/init.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/bas.h>

struct bas_bls {
    uint8_t flags;
    uint16_t power_state;
    uint16_t identifier;
    uint8_t battery_level;
} __packed;

enum {
    FLAGS_IDENTIFIER_PRESENT = BIT(0),
    FLAGS_BATTERY_LEVEL_PRESENT = BIT(1),
    FLAGS_ADDITIONAL_STATUS_PRESENT = BIT(2),
};

#define BATTERY_PRESENT_MASK GENMASK(0, 0)
#define WIRED_EXT_POWER_CONNECTED_MASK GENMASK(2, 1)
#define WIRELESS_EXT_POWER_CONNECTED_MASK GENMASK(4, 3)
#define BATTERY_CHARGE_STATE_MASK GENMASK(6, 5)
#define BATTERY_CHARGE_LEVEL_MASK GENMASK(8, 7)
#define CHARGING_TYPE_MASK GENMASK(11, 9)
#define CHARGING_FAULT_REASON_MASK GENMASK(14, 12)

static struct bas_bls battery_level_status = {
    .flags = FLAGS_IDENTIFIER_PRESENT | FLAGS_BATTERY_LEVEL_PRESENT,
    .power_state =
        FIELD_PREP(BATTERY_PRESENT_MASK, 1) | FIELD_PREP(WIRED_EXT_POWER_CONNECTED_MASK, 1) |
        FIELD_PREP(WIRELESS_EXT_POWER_CONNECTED_MASK, 0) |
        FIELD_PREP(BATTERY_CHARGE_STATE_MASK, 1) | FIELD_PREP(BATTERY_CHARGE_LEVEL_MASK, 2) |
        FIELD_PREP(CHARGING_TYPE_MASK, 2) | FIELD_PREP(CHARGING_FAULT_REASON_MASK, 0),
    .identifier = 0x0106,
    .battery_level = 0,
};

static ssize_t read_battery_level_status(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                         void *buf, uint16_t len, uint16_t offset) {
    const struct bas_bls *bls = attr->user_data;
    struct bas_bls value;

    value.flags = bls->flags;
    value.power_state = sys_cpu_to_le16(bls->power_state);
    value.identifier = sys_cpu_to_le16(bls->identifier);
    value.battery_level = bls->battery_level;

    LOG_WRN("ZMK BAS Read:");

    LOG_WRN("flags: %02X", value.flags);
    LOG_WRN("power_state: %04X", value.power_state);
    LOG_WRN("identifier: %04X", value.identifier);
    LOG_WRN("battery_level: %02X", value.battery_level);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &value, sizeof(value));
}

static void batery_level_status_ccc_changed(const struct bt_gatt_attr *_attr, uint16_t value) {
    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_DBG("ZMK BAS Status Notifications %s", notif_enabled ? "enabled" : "disabled");
}

BT_GATT_SERVICE_DEFINE(bas, BT_GATT_PRIMARY_SERVICE(BT_UUID_BAS),
                       BT_GATT_CHARACTERISTIC(BT_UUID_BAS_BATTERY_LEVEL_STATUS,
                                              BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ, read_battery_level_status, NULL,
                                              &battery_level_status),
                       BT_GATT_CCC(batery_level_status_ccc_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));

static int zmk_bas_init(void) { return 0; }

uint8_t zmk_bas_get_battery_level(void) { return battery_level_status.battery_level; }

int zmk_bas_set_battery_level(uint8_t level) {
    int rc;

    if (level > 100U) {
        return -EINVAL;
    }

    battery_level_status.battery_level = level;

    struct bas_bls value;

    value.flags = battery_level_status.flags;
    value.power_state = sys_cpu_to_le16(battery_level_status.power_state);
    value.identifier = sys_cpu_to_le16(battery_level_status.identifier);
    value.battery_level = level;

    LOG_WRN("ZMK BAS Set:");

    LOG_WRN("flags: %02X", value.flags);
    LOG_WRN("power_state: %04X", value.power_state);
    LOG_WRN("identifier: %04X", value.identifier);
    LOG_WRN("battery_level: %02X", value.battery_level);

    rc = bt_gatt_notify(NULL, &bas.attrs[1], &value, sizeof(value));

    return rc == -ENOTCONN ? 0 : rc;
}

int zmk_bas_set_charge_state(enum zmk_bas_charge_state charge_state) {
    battery_level_status.power_state |= FIELD_PREP(BATTERY_CHARGE_STATE_MASK, charge_state);

    int rc =
        bt_gatt_notify(NULL, &bas.attrs[1], &battery_level_status, sizeof(battery_level_status));

    return rc == -ENOTCONN ? 0 : rc;
}

SYS_INIT(zmk_bas_init, APPLICATION, CONFIG_ZMK_BLE_INIT_PRIORITY);
