#include "framework64/n64/framebuffer.h"

void fw64_framebuffer_set_pixel(fw64Framebuffer* framebuffer, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    int index = y * framebuffer->width + x;
    framebuffer->buffer[index] = GPACK_RGBA5551(r, g, b, 1);
}