#pragma once

/** \file camera.h */

#include "framework64/display.h"
#include "framework64/frustum.h"
#include "framework64/transform.h"
#include "framework64/types.h"
#include "framework64/vec2.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    fw64Transform transform;
    fw64Matrix projection;
    fw64Matrix view;
    float near, far;
    float fovy;
    float aspect;

    /** Do not write to this directly, instead use \ref fw64_camera_set_viewport or \ref fw64_camera_set_viewport_screen_relative */
    IVec2 viewport_pos;

    /** Do not write to this directly, instead use \ref fw64_camera_set_viewport or \ref fw64_camera_set_viewport_screen_relative */
    IVec2 viewport_size;
    fw64Display* display;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    Vp _viewport;
    uint16_t _persp_norm;
#endif
} fw64Camera;

void fw64_camera_init(fw64Camera* camera, fw64Display* display);
void fw64_camera_update_projection_matrix(fw64Camera* camera);
void fw64_camera_update_view_matrix(fw64Camera* camera);

void fw64_camera_extract_frustum_planes(fw64Camera* camera, fw64Frustum* planes);

/** Viewport's position and size in window space.  Note: Top Left is 0,0 */
void fw64_camera_set_viewport(fw64Camera* camera, IVec2* viewport_pos, IVec2* viewport_size);

/** Set Viewport rect proportional to the screen. position and size values should be in the range [0, 1]*/
void fw64_camera_set_viewport_relative(fw64Camera* camera, fw64Display* display, Vec2* viewport_position, Vec2* viewport_size);

int fw64_camera_ray_from_window_pos(fw64Camera* camera, IVec2* display_pos, Vec3* ray_origin, Vec3* ray_direction);
int fw64_camera_ray_from_viewport_pos(fw64Camera* camera, IVec2* viewport_pos, Vec3* ray_origin, Vec3* ray_direction);

#ifdef __cplusplus
}
#endif
