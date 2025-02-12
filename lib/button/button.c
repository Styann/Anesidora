#include "button.h"
#include "../pico_extra/pico_extra.h"

void button_init(const button_t *const self, const bool internal_pull_up) {
    gpio_init(self->pin);
    gpio_set_dir(self->pin, GPIO_IN);
    if (internal_pull_up) gpio_pull_up(self->pin);
}

bool button_debounce(button_t *const self) {
    if (millis() - self->timer > self->debounce_ms) {
        self->timer = millis();
        return true;
    }

    return false;
}

bool button_read(button_t *const self) {
    const bool lastState = self->state;
    self->state = !gpio_get(self->pin);

    self->has_state_changed = (self->state != lastState);

    return self->state;
}

bool button_is_pressed(button_t *const self) {
    return button_read(self);
}

bool button_is_released(button_t *const self) {
    return !button_read(self);
}

bool button_is_pressed_and_state_changed(button_t *const self) {
    return (button_is_pressed(self) && self->has_state_changed);
}

bool button_is_released_and_state_changed(button_t *const self) {
    return (button_is_released(self) && self->has_state_changed);
}
