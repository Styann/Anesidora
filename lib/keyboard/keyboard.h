/**
 * @author Styann
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "pico/stdlib.h"
#include "../usb/usb_hid.h"

typedef struct {
    const uint8_t *const layout;
    const uint8_t *const rows_pins;
    const uint8_t *const columns_pins;
    const uint row_length;
    const uint column_length;
} keyboard_matrix_t;

void keyboard_matrix_init(const keyboard_matrix_t *const self);

struct usb_keyboard_report keyboard_matrix_scan(const keyboard_matrix_t *const self);

static uint8_t get_modifier_from_keycode(const uint8_t keycode);

static void push_keycode(struct usb_keyboard_report *const report, const uint8_t keycode);

bool is_keyboard_report_empty(const struct usb_keyboard_report *const report);

bool keyboard_report_cmp(const struct usb_keyboard_report *const x, const struct usb_keyboard_report *const y);

bool is_gamepad_report_empty(const struct usb_gamepad_report *const report);

#endif
