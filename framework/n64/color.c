#include "ultra/color.h"

#include <string.h>

void color_set(Color* color, uint8_t r, uint8_t g, uint8_t b){
    color->r = r;
    color->g = g;
    color->b = b;
}

void color_assign(Color* dest, Color* src) {
    memcpy(dest, src, sizeof(Color));
}