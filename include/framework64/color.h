#ifndef FW64_COLOR_H
#define FW64_COLOR_H

#include <stdint.h>

typedef struct {
    uint8_t r, g, b;
} Color;

void color_set(Color* color, uint8_t r, uint8_t g, uint8_t b);
void color_assign(Color* dest, Color* src);

#endif
