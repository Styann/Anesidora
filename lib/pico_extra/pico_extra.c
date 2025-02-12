#include "pico_extra.h"

void utf8_to_utf16(const char *utf8, const uint utf8_len, uint16_t *utf16_buffer) {
    for (uint i = 0; i < utf8_len; i++) utf16_buffer[i] = utf8[i];
}
