#include "framework64/color.h"


void color_set(fw64ColorRGBA8* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}
