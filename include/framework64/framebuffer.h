#pragma once

/** \file framebuffer.h */

#include <stdint.h>

typedef struct fw64Framebuffer fw64Framebuffer;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_framebuffer_set_pixel(fw64Framebuffer* framebuffer, int x, int y, uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif
