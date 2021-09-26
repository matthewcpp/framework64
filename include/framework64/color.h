#pragma once

#include <stdint.h>

typedef struct {
    uint8_t r, g, b, a;
} fw64ColorRGBA8;

void fw64_color_rgba8_set(fw64ColorRGBA8* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
