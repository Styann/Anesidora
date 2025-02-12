#ifndef BUTTON_H
#define BUTTON_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"

typedef struct {
    uint8_t pin;
    bool state;
    bool has_state_changed;
    uint8_t debounce_ms;
    uint32_t timer;
} button_t;

void button_init(const button_t *const self, const bool internal_pull_up);

bool button_debounce(button_t *const self);

bool button_read(button_t *const self);

bool button_is_pressed(button_t *const self);

bool button_is_released(button_t *const self);

bool button_is_pressed_and_state_changed(button_t *const self);

bool button_is_released_and_state_changed(button_t *const self);

#endif
