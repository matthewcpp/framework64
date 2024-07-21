#pragma once

/** \file renderer_util.h */

#include "framework64/renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Renders a full screen overlay over the viewport.  This method can be useful for creating fade out effects.
 * Note: This method should be called right before ending render pass.
 * */
void fw64_renderer_util_fullscreen_overlay(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);


#ifdef __cplusplus
}
#endif
