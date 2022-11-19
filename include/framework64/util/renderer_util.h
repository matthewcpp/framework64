#pragma once

/** \file renderer_util.h */

#include "framework64/renderer.h"

#include "framework64/util/animated_mesh_configuration.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Renders a full screen overlay over the viewport.  This method can be useful for creating fade out effects.
 * Note: This method should be called right before ending render pass.
 * */
void fw64_renderer_util_fullscreen_overlay(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/** Clears a subset of the screen corresponding to the camera's viewport rectangle. */ 
void fw64_renderer_util_clear_viewport(fw64Renderer* renderer, fw64Camera* camera, fw64RendererFlags flags);

void fw64_renderer_util_draw_animated_mesh_configuration(fw64Renderer* renderer, fw64AnimatedMeshConfiguration* configuration, fw64Transform* transform, fw64AnimationController* animation_controller);

#ifdef __cplusplus
}
#endif
