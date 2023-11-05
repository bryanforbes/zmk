/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

int zmk_usb_hid_send_keyboard_report();
int zmk_usb_hid_send_consumer_report();
#ifdef CONFIG_ZMK_MOUSE
int zmk_usb_hid_send_mouse_report();
#endif
void zmk_usb_hid_set_protocol(uint8_t protocol);
