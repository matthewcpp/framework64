#pragma once

#include "framework64/renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Renders a full screen overlay over the viewport.  This method can be useful for creating fade out effects.
 * Note: This method should be called right before ending your triangle or line mode render pass.
 * */
void fw64_renderer_util_fullscreen_overlay(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/** Clears a subset of the screen corresponding to the camera's viewport rectangle. */ 
void fw64_renderer_util_clear_viewport(fw64Renderer* renderer, fw64Camera* camera, fw64RendererFlags flags);

#ifdef __cplusplus
}
#endif
