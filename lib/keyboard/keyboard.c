/**
 * @author Styann
 */

#include "keyboard.h"

#include <string.h>

#include "hardware/gpio.h"
#include "hardware/timer.h"

#include "../pico_extra/pico_extra.h"
#include "../usb/usb.h"

/**
 * @brief Set all rows pins as OUPUT and HIGH then all columns pins as GPIO INPUT PULL UP
 */
void keyboard_matrix_init(const keyboard_matrix_t *const self) {
    for (uint i = 0; i < self->row_length; i++) {
        gpio_init(self->rows_pins[i]);
        gpio_set_dir(self->rows_pins[i], GPIO_OUT);
        gpio_put(self->rows_pins[i], HIGH);
    }

    for (uint i = 0; i < self->column_length; i++) {
        gpio_init(self->columns_pins[i]);
        gpio_set_dir(self->columns_pins[i], GPIO_IN);
        gpio_pull_up(self->columns_pins[i]);
    }
}

/**
 * @brief loop through the matrix and add pressed keys to a keyboard report
 * @param matrix
 * @param handle_key_callback
 */
struct usb_keyboard_report keyboard_matrix_scan(const keyboard_matrix_t *const self) {
    struct usb_keyboard_report report = { 0x01, 0, 0, { 0, 0, 0, 0, 0, 0 } };
    uint8_t keycodes_length = 0;

    for (uint row_i = 0; (keycodes_length < sizeof(report.keycodes)) && (row_i < self->row_length); row_i++) {
        gpio_put(self->rows_pins[row_i], LOW); // low output pin act as a ground
        busy_wait_us_32(5); // waiting for the trace to be physically low

        for (uint column_i = 0; (keycodes_length < sizeof(report.keycodes)) && (column_i < self->column_length); column_i++) {
            if (!gpio_get(self->columns_pins[column_i])) {
                const uint8_t keycode = self->layout[row_i * self->column_length + column_i];

                if (keycode == KC_NONE) {
                    continue;
                }
                else if (keycode >= KC_CTRL_LEFT && keycode <= KC_GUI_RIGHT) {
                    report.modifiers |= get_modifier_from_keycode(keycode);
                }
                else {
                    push_keycode(&report, keycode);
                    keycodes_length++;
                }
            }
        }

        gpio_put(self->rows_pins[row_i], HIGH);
    }

    return report;
}

/**
 * @brief take a keycode and return his modifier if he has one
 * @param keycode must be between KC_CTRL_LEFT (0xE0) and KC_GUI_RIGHT (0xE7)
 */
static uint8_t get_modifier_from_keycode(const uint8_t keycode) {
    return (0x01 << (keycode & 0b00001111));
}

/**
 * @brief push it into keycodes array
 * @param report
 * @param keycode
 */
static void push_keycode(struct usb_keyboard_report *const report, const uint8_t keycode) {
     for (uint8_t i = 0; i < sizeof(report->keycodes); i++) {
        if (report->keycodes[i] == KC_NONE) {
            report->keycodes[i] = keycode;
            break;
        }
    }
}

/**
 * @brief return true if modifiers and keycodes are set to 0, else false
 * @param keyboard_report
 */
bool is_keyboard_report_empty(const struct usb_keyboard_report *const report) {
    if (report->modifiers != MOD_NONE) return false;

    for (uint8_t i = 0; i < sizeof(report->keycodes); i++) {
        if (report->keycodes[i] != KC_NONE) return false;
    }

    return true;
}

bool keyboard_report_cmp(const struct usb_keyboard_report *const x, const struct usb_keyboard_report *const y) {
    return (memcmp(x, y, sizeof(struct usb_keyboard_report)) == 0) ? true : false;
}

bool is_gamepad_report_empty(const struct usb_gamepad_report *const report) {
    return !(report->x | report->y | report->buttons);
}
