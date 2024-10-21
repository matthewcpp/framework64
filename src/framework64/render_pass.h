#pragma once

/** \file render_pass.h */

#include "framework64/allocator.h"
#include "framework64/animation_controller.h"
#include "framework64/camera.h"
#include "framework64/color.h"
#include "framework64/display.h"
#include "framework64/mesh_instance.h"
#include "framework64/skinned_mesh_instance.h"
#include "framework64/sprite_batch.h"

typedef struct fw64RenderPass fw64RenderPass;

#ifdef FW64_PLATFORM_N64_LIBDRAGON
#include <GL/gl_enums.h>
typedef enum {
    FW64_CLEAR_FLAG_NONE = 0,
    FW64_CLEAR_FLAG_COLOR = GL_COLOR_BUFFER_BIT,
    FW64_CLEAR_FLAG_DEPTH = GL_DEPTH_BUFFER_BIT,
    FW64_CLEAR_FLAG_ALL = FW64_CLEAR_FLAG_COLOR | FW64_CLEAR_FLAG_DEPTH
} fw64ClearFlags;
#else
typedef enum {
    FW64_CLEAR_FLAG_NONE = 0,
    FW64_CLEAR_FLAG_COLOR = 1,
    FW64_CLEAR_FLAG_DEPTH = 2,
    FW64_CLEAR_FLAG_ALL = FW64_CLEAR_FLAG_COLOR | FW64_CLEAR_FLAG_DEPTH
} fw64ClearFlags;
#endif

#ifdef __cplusplus
extern "C" {
#endif

fw64RenderPass* fw64_renderpass_create(fw64Display* display, fw64Allocator* allocator);
void fw64_renderpass_delete(fw64RenderPass* renderpass);

void fw64_renderpass_begin(fw64RenderPass* renderpass);
void fw64_renderpass_end(fw64RenderPass* renderpass);

void fw64_renderpass_set_view_matrix(fw64RenderPass* pass, float* view);
void fw64_renderpass_set_projection_matrix(fw64RenderPass* pass, float* projection, uint16_t* persp_norm_ptr);
void fw64_renderpass_set_viewport(fw64RenderPass* pass, const fw64Viewport* viewport);
const fw64Viewport* fw64_renderpass_get_viewport(fw64RenderPass* renderpass);
void fw64_renderpass_set_camera(fw64RenderPass* pass, fw64Camera* camera);

void fw64_renderpass_set_depth_testing_enabled(fw64RenderPass* renderpass, int enabled);

void fw64_renderpass_set_clear_flags(fw64RenderPass* pass, fw64ClearFlags flags);
void fw64_renderpass_set_clear_color(fw64RenderPass* pass, uint8_t r, uint8_t g, uint8_t b);

void fw64_renderpass_draw_sprite_batch(fw64RenderPass* renderpass, fw64SpriteBatch* text_batch);
void fw64_renderpass_draw_static_mesh(fw64RenderPass* renderpass, fw64MeshInstance* mesh_instance);
void fw64_renderpass_draw_skinned_mesh(fw64RenderPass* renderpass, fw64SkinnedMeshInstance* skinned_mesh_instance);

void fw64_renderpass_set_fog_enabled(fw64RenderPass* renderpass, int enabled);
/**
 * Sets the min and max positions of fog.
 * The values for min / max should be values in the range [0.0, 1.0]
 * A values of 0.0 is on the near plane, while 1.0 is on the far plane.
 */ 
void fw64_renderpass_set_fog_positions(fw64RenderPass* renderpass, float fog_min, float fog_max);
void fw64_renderpass_set_fog_color(fw64RenderPass* renderpass, uint8_t r, uint8_t g, uint8_t b);

void fw64_renderpass_set_light_enabled(fw64RenderPass* renderpass, int index, int enabled);
void fw64_renderpass_set_light_direction(fw64RenderPass* renderpass, int index, Vec3* direction);
void fw64_renderpass_set_light_color(fw64RenderPass* renderpass, int index, fw64ColorRGBA8 color);
void fw64_renderpass_set_lighting_ambient_color(fw64RenderPass* renderpass, fw64ColorRGBA8 color);

#ifdef __cplusplus
}
#endif
