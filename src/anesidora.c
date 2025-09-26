/**
 * @author Styann
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/structs/usb.h"
#include "hardware/uart.h"

#include "../lib/pico_extra/pico_extra.h"
#include "../lib/usb/usb.h"
#include "../lib/usb/usb_hid.h"
#include "../lib/debounce/debounce.h"
#include "../lib/keyboard/keyboard.h"
#include "../lib/rotary_encoder/rotary_encoder.h"
#include "../lib/graphics/color.h"
#include "../lib/ws2812b/ws2812b.h"
#include "../lib/ssd1306/ssd1306.h"
#include "../lib/led/led.h"

#include "../include/bongocat.h"
#include "../include/usb_descriptors.h"

#define USE_CAPSLOCK_LED
#define USE_KEYBOARD
#define USE_KNOB
#define USE_WS2812B
#define USE_SSD1306

enum multicore_flags {
    BONGOCAT_TAP_FLAG = 1,
    BONGOCAT_PAWS_UP_FLAG,
    USB_SUSPEND_FLAG,
    USB_RESUME_FLAG,
    LEDS_CHANGE_COLOR,
    GAME_MODE
};

void ep0_out_handler(uint8_t *buf, uint16_t len);
void ep0_in_handler(uint8_t *buf, uint16_t len);
void ep1_in_handler(uint8_t *buf, uint16_t len);

usb_device_t pico = {
    .addr = 0x00,
    .should_set_addr = false,
    .suspended = false,
    .configured = false,
    .endpoints = {
        {
            .descriptor = &ep0_in_descriptor,
            .handler = &ep0_in_handler,
            .endpoint_control = NULL, // NA for EP0,
            .buffer_control = &usb_dpram->ep_buf_ctrl[0].in,
            // EP0 in and out share a data buffer
            .data_buffer = &usb_dpram->ep0_buf_a[0],
        },
        {
            .descriptor = &ep0_out_descriptor,
            .handler = &ep0_out_handler,
            .endpoint_control = NULL, // NA for EP0
            .buffer_control = &usb_dpram->ep_buf_ctrl[0].out,
            // EP0 in and out share a data buffer
            .data_buffer = &usb_dpram->ep0_buf_a[0],
        },
        {
            .descriptor = &ep1_in_descriptor,
            .handler = &ep1_in_handler,
            .endpoint_control = &usb_dpram->ep_ctrl[0].in,
            .buffer_control = &usb_dpram->ep_buf_ctrl[1].in,
            .data_buffer = &usb_dpram->epx_data[0 * 64]
        }
    },
    .device_descriptor = &device_descriptor,
    .configuration_descriptor = &config_descriptor,
    .language_descriptor = &language_descriptor,
    .vendor = "Seegson Electronics Corp.",
    .product = "Anesidora Keyboard",
};

void ep0_out_handler(uint8_t *buf, uint16_t len) {

}

void ep0_in_handler(uint8_t *buf, uint16_t len) {
    if (pico.should_set_addr) {
        // Set actual device address in hardware
        usb_hw->dev_addr_ctrl = pico.addr;
        pico.should_set_addr = false;
    } else {
        // Receive a zero length status packet from the host on EP0 OUT
        usb_xfer(usb_get_endpoint(&pico, USB_DIR_OUT), NULL, 0);
    }
}

void ep1_in_handler(uint8_t *buf, uint16_t len) {

}

struct usb_hid_interface keyboard_interface = {
    .interface_descriptor = &keyboard_interface_descriptor,
    .hid_descriptor = &keyboard_hid_descriptor,
    .endpoint = &pico.endpoints[2]
};

void usb_handle_hid_report(const uint8_t bDescriptorIndex, const uint16_t wInterfaceNumber, const uint16_t wDescriptorLength) {
    if (bDescriptorIndex == 0 && wInterfaceNumber == 0) {
        usb_xfer_ep0_in(&pico, (uint8_t *)keyboard_report_descriptor, sizeof(keyboard_report_descriptor));
    }
}

void usb_handle_config_descriptor(usb_device_t *const device, const uint16_t wLength) {
    uint8_t buffer[128];
    uint8_t *buffer_ptr = buffer;

    memcpy((void *)buffer_ptr, device->configuration_descriptor, device->configuration_descriptor->bLength);
    buffer_ptr += device->configuration_descriptor->bLength;

    if (wLength >= device->configuration_descriptor->wTotalLength) {
        buffer_ptr += hid_interface_cpy(buffer_ptr, &keyboard_interface);
    }

    usb_xfer_ep0_in(device, buffer, buffer_ptr - buffer);
}

struct usb_endpoint *const hid_ep = &pico.endpoints[2];

/**
 * @brief USB interrupt handler
 */
void isr_usbctrl(void) {
    isr_usbctrl_task(&pico);
}

void usb_suspend_callback(void) {
    multicore_fifo_push_blocking(USB_SUSPEND_FLAG);
}

void usb_resume_callback(void) {
    multicore_fifo_push_blocking(USB_RESUME_FLAG);
}

#ifdef USE_CAPSLOCK_LED
led_t capslock_led = { .pin = GPIO27, .state = LOW };
#endif

#ifdef USE_SSD1306
ssd1306_t output = {
    .pin_SCK = GPIO21,
    .pin_SDA = GPIO20,
    .i2c_inst = i2c0
};
#endif

#ifdef USE_WS2812B
#define NUM_LEDS 16

struct grb leds_buffer[NUM_LEDS];
uint8_t leds_spi_buffer[NUM_LEDS][bitsizeof(struct grb)];

struct ws2812b led_strip = {
    .pin_Din = GPIO19,
    .spi_inst = spi0,
    .num_leds = NUM_LEDS,
    .leds_buffer = leds_buffer,
    .spi_buffer = &leds_spi_buffer[0][0],
};
#endif

volatile uint8_t modifiers = 0b00000000;
volatile bool fn_key = false;

#ifdef USE_KNOB
void knob_ccw_callback(const uint32_t state);
void knob_cw_callback(const uint32_t state);

rotary_encoder_t volume_knob = {
    .pin_DT = GPIO22,
    .pin_CLK = GPIO26,
    .cw_callback = &knob_cw_callback,
    .ccw_callback = &knob_ccw_callback
};

void knob_ccw_callback(const uint32_t state) {
    if (fn_key) {
        usb_send_consumer_control(hid_ep, USB_CONSUMER_CONTROL_BRIGHTNESS_DECREMENT);
    }
    else {
        usb_send_consumer_control(hid_ep, USB_CONSUMER_CONTROL_VOLUME_DECREMENT);
    }
}

void knob_cw_callback(const uint32_t state) {
    if (fn_key) {
        usb_send_consumer_control(hid_ep, USB_CONSUMER_CONTROL_BRIGHTNESS_INCREMENT);
    }
    else {
        usb_send_consumer_control(hid_ep, USB_CONSUMER_CONTROL_VOLUME_INCREMENT);
    }
}
#endif

#ifdef USE_KEYBOARD
#define USB_KEY_FN 0xA5 // 0xA5 is reserved, so I can use it as FN key
#define DEBOUNCE_MS 8
#define LAYOUT_COLUMN_LENGTH 8
#define LAYOUT_ROW_LENGTH 9

const uint8_t columns_pins[LAYOUT_COLUMN_LENGTH] = { GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, GPIO6, GPIO7 };
const uint8_t rows_pins[LAYOUT_ROW_LENGTH] = { GPIO8, GPIO9, GPIO10, GPIO11, GPIO12, GPIO13, GPIO14, GPIO15, GPIO18 };

uint8_t layout[LAYOUT_ROW_LENGTH][LAYOUT_COLUMN_LENGTH] = {
    { USB_KEY_ESCAPE,     USB_KEY_1,        USB_KEY_2,        USB_KEY_3,                   USB_KEY_4,                    USB_KEY_5,         USB_KEY_6,           USB_KEY_7          },
    { USB_KEY_TAB,        USB_KEY_A,        USB_KEY_Z,        USB_KEY_E,                   USB_KEY_R,                    USB_KEY_T,         USB_KEY_Y,           USB_KEY_U          },
    { USB_KEY_CAPS_LOCK,  USB_KEY_Q,        USB_KEY_S,        USB_KEY_D,                   USB_KEY_F,                    USB_KEY_G,         USB_KEY_H,           USB_KEY_J          },
    { USB_KEY_SHIFT_LEFT, USB_KEY_W,        USB_KEY_X,        USB_KEY_C,                   USB_KEY_V,                    USB_KEY_B,         USB_KEY_N,           USB_KEY_SEMICOLON  },
    { USB_KEY_CTRL_LEFT,  USB_KEY_ALT_LEFT, USB_KEY_GUI_LEFT, USB_KEY_SPACE,               USB_KEY_ALT_RIGHT,            USB_KEY_FN,        USB_KEY_CTRL_RIGHT,  USB_KEY_ARROW_LEFT },
    { USB_KEY_8,          USB_KEY_9,        USB_KEY_0,        USB_KEY_MINUS,               USB_KEY_EQUAL,                USB_KEY_BACKSPACE, USB_KEY_GRAVE,       USB_KEY_PAUSE      },
    { USB_KEY_I,          USB_KEY_O,        USB_KEY_P,        USB_KEY_SQUARE_BRACKET_LEFT, USB_KEY_SQUARE_BRACKET_RIGHT, USB_KEY_BACKSLASH, USB_KEY_DELETE,      USB_KEY_NONE       },
    { USB_KEY_K,          USB_KEY_L,        USB_KEY_M,        USB_KEY_APOSTROPHE,          USB_KEY_ENTER,                USB_KEY_PAGE_UP,   USB_KEY_ARROW_RIGHT, USB_KEY_NONE       },
    { USB_KEY_COMMA,      USB_KEY_PERIOD,   USB_KEY_SLASH,    USB_KEY_SHIFT_RIGHT,         USB_KEY_ARROW_UP,             USB_KEY_PAGE_DOWN, USB_KEY_ARROW_DOWN , USB_KEY_NONE       },
};

const keyboard_matrix_t keyboard_matrix = {
    .layout = &layout[0][0],
    .columns_pins = columns_pins,
    .rows_pins = rows_pins,
    .row_length = LAYOUT_ROW_LENGTH,
    .column_length = LAYOUT_COLUMN_LENGTH
};

void handle_fn_key(struct usb_keyboard_report *const report) {
    fn_key = false;

    for (uint8_t i = 0; i < sizeof(report->keycodes); i++) {
        if (report->keycodes[i] == USB_KEY_FN) {
            report->keycodes[i] = USB_KEY_NONE;
            fn_key = true;

            for (uint8_t j = i; j < (sizeof(report->keycodes) - 1); j++) {
                report->keycodes[j] = report->keycodes[j + 1];
            }

            break;
        }
    }
}

/**
 * @returns {bool} if true, should not send report
 */
bool macro_task(struct usb_keyboard_report *report) {
    bool match = false;

    if (fn_key) {
        switch (report->keycodes[0]) {
            case USB_KEY_PAGE_UP:
                report->keycodes[0] = USB_KEY_HOME;
                break;
            case USB_KEY_PAGE_DOWN:
                report->keycodes[0] = USB_KEY_END;
                break;
            case USB_KEY_S:
                report->keycodes[0] = USB_KEY_PRINT_SCREEN;
                break;
            case USB_KEY_I:
                report->keycodes[0] = USB_KEY_F12; // to toggle inspector in browser
                break;
            case USB_KEY_F:
                report->keycodes[0] = USB_KEY_F11; // fullscreen
                break;
            case USB_KEY_G:
                layout[4][2] = (layout[4][2] == USB_KEY_GUI_LEFT) ? USB_KEY_ALT_RIGHT : USB_KEY_GUI_LEFT;
                multicore_fifo_push_blocking(GAME_MODE);
                match = true;
                break;
            case USB_KEY_L:
                multicore_fifo_push_blocking(LEDS_CHANGE_COLOR);
                match = true;
                break;
            case USB_KEY_P:
                usb_send_consumer_control(hid_ep, USB_CONSUMER_CONTROL_PLAY_PAUSE);
                match = true;
                break;
            case USB_KEY_PERIOD:
                usb_send_consumer_control(hid_ep, USB_CONSUMER_CONTROL_NEXT_TRACK);
                match = true;
                break;
            case USB_KEY_COMMA:
                usb_send_consumer_control(hid_ep, USB_CONSUMER_CONTROL_PREVIOUS_TRACK);
                match = true;
                break;
            default:
                if (report->keycodes[0] >= USB_KEY_1 && report->keycodes[0] <= USB_KEY_0) {
                    report->keycodes[0] += (USB_KEY_F1 - USB_KEY_1);
                }
        }
    }
    else {
        if (report->keycodes[0] == USB_KEY_PAUSE) {
            usb_send_consumer_control(hid_ep, USB_CONSUMER_CONTROL_PLAY_PAUSE);
            match = true;
        }
    }

    return match;
}

void keyboard_task(void) {
    static uint32_t timer = 0;

    if (debounce(&timer, DEBOUNCE_MS)) {
        static bool can_release = false;
        static struct usb_keyboard_report previous_report = { 0x01, 0, 0, { 0, 0, 0, 0, 0, 0 }};

        struct usb_keyboard_report keyboard_report = keyboard_matrix_scan(&keyboard_matrix);

        handle_fn_key(&keyboard_report);
        modifiers = keyboard_report.modifiers;

        // if there is a change between actual and previous report
        if (!keyboard_report_cmp(&keyboard_report, &previous_report)) {
            if (!macro_task(&keyboard_report)) {
                if (!is_keyboard_report_empty(&keyboard_report)) {
                    if (pico.suspended) {
                        usb_resume_callback();
                        pico.suspended = false;
                    }

                    usb_send_keyboard_report(hid_ep, &keyboard_report);
                    can_release = true;

                    #ifdef USE_SSD1306
                    multicore_fifo_push_blocking(BONGOCAT_TAP_FLAG);
                    #endif
                }
                else if (can_release) {
                    release_keyboard(hid_ep);
                    can_release = false;

                    #ifdef USE_SSD1306
                    multicore_fifo_push_blocking(BONGOCAT_PAWS_UP_FLAG);
                    #endif
                }
            }

            previous_report = keyboard_report;
        }
    }
}
#endif

void main_core1(void) {
    uint8_t color_index = 0;
    bool game_mode = false;

    const struct grb colors[] = {
        grb(255, 0, 0),
        grb(251, 255, 0), // yellow
        grb(39, 238, 255), // magenta
        grb(255, 39, 255), // cyan
        grb(128, 255, 0), // orange
        grb(0, 0, 0)
    };

    core1_resume: {
        #ifdef USE_SSD1306
        ssd1306_init(&output, 400000);
        ssd1306_write_all(&output, bongocat_paws_up);
        uint32_t bongocat_paw_state = 0;
        uint32_t bongocat_timer = 0;
        #endif

        #ifdef USE_WS2812B
        ws2812b_init(&led_strip);
        ws2812b_write(&led_strip);
        #endif

        while (true) {
            uint32_t multicore_flag = (multicore_fifo_rvalid()) ? multicore_fifo_pop_blocking() : 0;

            switch (multicore_flag) {
                case USB_SUSPEND_FLAG:
                    goto core1_suspend;
                    break;
                case BONGOCAT_TAP_FLAG:
                    #ifdef USE_SSD1306
                    ssd1306_write_all(&output, bongocat_paw_down[bongocat_paw_state]);
                    bongocat_paw_state ^= 1;
                    multicore_flag = 0;
                    #endif
                    break;
                case BONGOCAT_PAWS_UP_FLAG:
                    #ifdef USE_SSD1306
                    ssd1306_write_all(&output, bongocat_paws_up);
                    multicore_flag = 0;
                    break;
                    #endif
                case LEDS_CHANGE_COLOR:
                    #ifdef USE_WS2812B
                    color_index++;
                    if (color_index > (lengthof(colors) - 1)) color_index = 0;

                    ws2812b_set_all(&led_strip, colors[color_index]);
                    ws2812b_write(&led_strip);
                    #endif
                    break;
                case GAME_MODE:
                    #ifdef USE_SSD1306
                    ssd1306_set_addr(&output, 3, 0);

                    if (game_mode) {
                        ssd1306_print(&output, " ");
                        game_mode = false;
                    }
                    else {
                        ssd1306_print(&output, "G");
                        game_mode = true;
                    }
                    #endif
            }
        }
    }

    core1_suspend: {
        #ifdef USE_SSD1306
        ssd1306_deinit(&output);
        #endif

        #ifdef USE_WS2812B
        ws2812b_set_all(&led_strip, grb(0, 0, 0));
        ws2812b_write(&led_strip);
        ws2812b_deinit(&led_strip);
        #endif

        while (true) {
            uint32_t flag = (multicore_fifo_rvalid()) ? multicore_fifo_pop_blocking() : 0;

            if (flag == USB_RESUME_FLAG) {
                goto core1_resume;
            }
        }
    }
}

void set_report_callback(volatile uint8_t *buf, uint16_t len) {
    usb_acknowledge_out_request(&pico);
}

int main(void) {
    built_in_led_init();
    built_in_led_on();

    uart_init(uart0, 115200);
    gpio_set_function(GPIO16, UART_FUNCSEL_NUM(uart0, GPIO16));
    gpio_set_function(GPIO17, UART_FUNCSEL_NUM(uart0, GPIO17));
    uart_puts(uart0, "initing pico !\n");

    #ifdef USE_CAPSLOCK_LED
    led_init(&capslock_led);
    #endif

    usb_device_init(&pico);

    while (!pico.configured) {
        tight_loop_contents();
    }

    built_in_led_off();

    multicore_launch_core1(&main_core1);

    #ifdef USE_KEYBOARD
    keyboard_matrix_init(&keyboard_matrix);
    #endif

    #ifdef USE_KNOB
    rotary_encoder_init(&volume_knob, false);
    #endif

    while (true) {
        #ifdef USE_KEYBOARD
        keyboard_task();
        #endif

        #ifdef USE_KNOB
        rotary_encoder_task(&volume_knob);
        #endif
    }

    return 0;
}
