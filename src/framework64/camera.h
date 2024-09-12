#pragma once

/** \file camera.h */

#include "framework64/display.h"
#include "framework64/frustum.h"
#include "framework64/transform.h"
#include "framework64/types.h"
#include "framework64/viewport.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FW64_PLATFORM_N64_LIBULTRA
#define FW64_N64_LIBULTRA_DEFAULT_PERSPNORM 262
#endif

typedef enum  {
    FW64_CAMERA_PROJECTION_PERSPECTIVE,
    FW64_CAMERA_PROJECTION_ORTHOGRAPHIC
} fw64CameraProjectionMode;

typedef struct {
    fw64Transform transform;
    fw64Matrix projection;
    fw64Matrix view;
    float near, far;
    float fovy;
    float aspect;
    float ortho_size;
    fw64CameraProjectionMode projection_mode;

    fw64Display* display;
    /** Do not write to this directly, instead use \ref fw64_camera_set_viewport or \ref fw64_camera_set_viewport_screen_relative */
    fw64Viewport viewport;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    uint16_t _persp_norm;
    uint16_t _unused;
#endif
} fw64Camera;

void fw64_camera_init(fw64Camera* camera, fw64Display* display);
void fw64_camera_init_persp(fw64Camera* camera, fw64Display* display);
void fw64_camera_init_ortho(fw64Camera* camera, fw64Display* display);

void fw64_camera_update_projection_matrix(fw64Camera* camera);
void fw64_camera_update_view_matrix(fw64Camera* camera);

void fw64_camera_extract_frustum_planes(fw64Camera* camera, fw64Frustum* planes);

/** This function return NULL on all non-n64 platforms */
uint16_t* fw64_camera_get_perspnorm(fw64Camera* camera);

/** Viewport's position and size in window space.  Note: Top Left is 0,0 */
void fw64_camera_set_viewport(fw64Camera* camera, const IVec2* viewport_pos, const IVec2* viewport_size);

/** Set Viewport rect proportional to the screen. position and size values should be in the range [0, 1]*/
void fw64_camera_set_viewport_relative(fw64Camera* camera, Vec2* viewport_pos, Vec2* viewport_size);

int fw64_camera_ray_from_window_pos(fw64Camera* camera, IVec2* display_pos, Vec3* ray_origin, Vec3* ray_direction);
int fw64_camera_ray_from_viewport_pos(fw64Camera* camera, IVec2* viewport_pos, Vec3* ray_origin, Vec3* ray_direction);

int fw64_camera_world_to_viewport_pos(fw64Camera* camera, Vec3* world_pos, IVec2* viewport_pos);

#ifdef __cplusplus
}
#endif
