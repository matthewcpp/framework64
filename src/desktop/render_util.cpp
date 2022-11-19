#include "framework64/util/renderer_util.h"

#include "framework64/desktop/renderer.h"

void fw64_renderer_util_fullscreen_overlay(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    renderer->renderFullscreenOverlay(r, g, b, a);
}

void fw64_renderer_util_clear_viewport(fw64Renderer* renderer, fw64Camera* camera, fw64RendererFlags flags) {
    renderer->clearViewport(camera, flags);
}

void fw64_renderer_util_draw_animated_mesh_configuration(fw64Renderer* renderer, fw64AnimatedMeshConfiguration* configuration, fw64Transform* transform, fw64AnimationController* animation_controller) {
    renderer->drawAnimatedMeshConfiguration(configuration, transform, animation_controller);
}
