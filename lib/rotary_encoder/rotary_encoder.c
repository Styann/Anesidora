/**
 * @author Styann
 */

#include "rotary_encoder.h"

#include "hardware/gpio.h"

#include "../pico_extra/pico_extra.h"

void rotary_encoder_init(rotary_encoder_t *const self, const bool use_internal_pull_up) {
    self->state = 0;
    self->state_CLK = LOW;

    gpio_init(self->pin_CLK);
    gpio_init(self->pin_DT);

    gpio_set_dir(self->pin_CLK, GPIO_IN);
    gpio_set_dir(self->pin_DT, GPIO_IN);

    if (use_internal_pull_up) {
        gpio_pull_up(self->pin_CLK);
        gpio_pull_up(self->pin_DT);
    }
}

/**
 * @brief read and return direction
 * @returns {int8_t} clockwise: 1, unmoving: 0, counter_clockwise: -1
 */
int8_t rotary_encoder_read(rotary_encoder_t *const self) {
    const bool last_state_CLK = self->state_CLK;
    self->state_CLK = gpio_get(self->pin_CLK);

    if (self->state_CLK != last_state_CLK) {
        if (gpio_get(self->pin_DT) != self->state_CLK) {
            return 1;
        }
        else {
            return -1;
        }
    }

    return 0;
}

/**
 * @brief read direction, then execute cw or ccw callback
 */
void rotary_encoder_task(rotary_encoder_t *const self) {
    const bool last_state_CLK = self->state_CLK;
    self->state_CLK = gpio_get(self->pin_CLK);

    if (self->state_CLK != last_state_CLK) {
        if (gpio_get(self->pin_DT) != self->state_CLK) {
            self->cw_callback(++self->state);
        }
        else {
            self->ccw_callback(--self->state);
        }
    }
}
