#pragma once

/** \file render_pass.h */

#include "framework64/allocator.h"
#include "framework64/animation_controller.h"
#include "framework64/camera.h"
#include "framework64/color.h"
#include "framework64/display.h"
#include "framework64/mesh.h"
#include "framework64/sprite_batch.h"

typedef struct fw64RenderPass fw64RenderPass;

typedef enum {
    FW64_CLEAR_FLAG_NONE = 0,
    FW64_CLEAR_FLAG_COLOR = 1,
    FW64_CLEAR_FLAG_DEPTH = 2,
    FW64_CLEAR_FLAG_ALL = FW64_CLEAR_FLAG_COLOR | FW64_CLEAR_FLAG_DEPTH
} fw64ClearFlags;

#ifdef __cplusplus
extern "C" {
#endif

fw64RenderPass* fw64_renderpass_create(fw64Display* display, fw64Allocator* allocator);
void fw64_renderpass_delete(fw64RenderPass* renderpass);

void fw64_renderpass_begin(fw64RenderPass* renderpass);
void fw64_renderpass_end(fw64RenderPass* renderpass);

void fw64_renderpass_set_view_matrix(fw64RenderPass* pass, float* view);
void fw64_renderpass_set_projection_matrix(fw64RenderPass* pass, float* projection, uint16_t* persp_norm_ptr);
void fw64_renderpass_set_viewport(fw64RenderPass* pass, fw64Viewport* viewport);
void fw64_renderpass_set_camera(fw64RenderPass* pass, fw64Camera* camera);

void fw64_renderpass_set_depth_testing_enabled(fw64RenderPass* renderpass, int enabled);

void fw64_renderpass_set_clear_flags(fw64RenderPass* pass, fw64ClearFlags flags);
void fw64_renderpass_set_clear_color(fw64RenderPass* pass, uint8_t r, uint8_t g, uint8_t b);

void fw64_renderpass_draw_sprite_batch(fw64RenderPass* renderpass, fw64SpriteBatch* text_batch);
void fw64_renderpass_draw_static_mesh(fw64RenderPass* renderpass, fw64Mesh* mesh, fw64Transform* transform);
void fw64_renderpass_draw_animated_mesh(fw64RenderPass* renderpass, fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform);
#ifdef __cplusplus
}
#endif
