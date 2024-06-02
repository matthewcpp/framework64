#include "framework64/camera.h"

#include "framework64/matrix.h"
#include "framework64/matrix_project.h"

#include <string.h>

static void camera_common_init(fw64Camera* camera, fw64Display* display);

void fw64_camera_init(fw64Camera* camera, fw64Display* display) {
    fw64_camera_init_persp(camera, display);
}

void fw64_camera_init_persp(fw64Camera* camera, fw64Display* display){
    camera->projection_mode = FW64_CAMERA_PROJECTION_PERSPECTIVE;
    camera_common_init(camera, display);
}

void fw64_camera_init_ortho(fw64Camera* camera, fw64Display* display) {
    camera->projection_mode = FW64_CAMERA_PROJECTION_ORTHOGRAPHIC;
    camera_common_init(camera, display);
}

void camera_common_init(fw64Camera* camera, fw64Display* display) {
    fw64_transform_init(&camera->transform);
    camera->display = display;

    camera->near = 0.1f;
    camera->far = 500.0f;
    camera->fovy = 45.0f;
    camera->aspect = 1.33f;
    camera->transform.position.z = 5.0f;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    camera->_persp_norm = FW64_N64_LIBULTRA_DEFAULT_PERSPNORM;
#endif

    camera->ortho_size = 5.0f;

    fw64_camera_update_projection_matrix(camera);
    fw64_camera_update_view_matrix(camera);

    IVec2 pos = {0.0f, 0.0f};
    IVec2 size = fw64_display_get_size(display);
    fw64_camera_set_viewport(camera, &pos, &size);
}

void fw64_camera_update_projection_matrix(fw64Camera* camera) {
    float ortho_size_v = camera->aspect * camera->ortho_size;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    if (camera->projection_mode == FW64_CAMERA_PROJECTION_PERSPECTIVE) {
        guPerspective(&camera->projection, &camera->_persp_norm, 
            camera->fovy, camera->aspect, camera->near, camera->far, 1.0f);
    } else {
        guOrtho(&camera->projection, -ortho_size_v, ortho_size_v, -camera->ortho_size, camera->ortho_size, camera->near, camera->far, 1.0f);
    }
#else
    if (camera->projection_mode == FW64_CAMERA_PROJECTION_PERSPECTIVE) {
        matrix_perspective(&camera->projection.m[0], camera->fovy, camera->aspect, camera->near, camera->far);
    } else {
        matrix_ortho(&camera->projection.m[0], -ortho_size_v, ortho_size_v, -camera->ortho_size, camera->ortho_size, camera->near, camera->far);
    }
#endif
}

void fw64_camera_update_view_matrix(fw64Camera* camera) {
    Vec3 forward, up, target;
    fw64_transform_forward(&camera->transform, &forward);
    fw64_transform_up(&camera->transform, &up);

    vec3_add(&target, &camera->transform.position, &forward);

#ifdef FW64_PLATFORM_N64_LIBULTRA
    guLookAt(&camera->view, 
    camera->transform.position.x, camera->transform.position.y, camera->transform.position.z,
    target.x, target.y, target.z, up.x, up.y, up.z);
#else
    matrix_camera_look_at(&camera->view.m[0], &camera->transform.position, &target, &up);
#endif
}

void fw64_camera_extract_frustum_planes(fw64Camera* camera, fw64Frustum* frustum) {
    float projScreenMatrix[16];

    #ifdef FW64_PLATFORM_N64_LIBULTRA
    Mtx projViewMatrix;
    guMtxCatL(&camera->view, &camera->projection, &projViewMatrix);
    guMtxL2F((void*)&projScreenMatrix[0], &projViewMatrix);
    #else
    matrix_multiply(&projScreenMatrix[0], &camera->projection.m[0], &camera->view.m[0]);
    #endif

    fw64_frustum_set_from_matrix(frustum, &projScreenMatrix[0]);
}

void fw64_camera_set_viewport(fw64Camera* camera, IVec2* viewport_pos, IVec2* viewport_size) {
    fw64_viewport_set(&camera->viewport, viewport_pos, viewport_size);
}

void fw64_camera_set_viewport_relative(fw64Camera* camera, Vec2* viewport_pos, Vec2* viewport_size) {
    fw64_viewport_set_relative_to_display(&camera->viewport, camera->display, viewport_pos, viewport_size);
}


// this method will go away when matrices are fixed up on the camera
static void _temp_camera_compute_view_projection(fw64Camera* camera, float* view, float* proj) {
    matrix_perspective(proj, camera->fovy, camera->aspect, camera->near, camera->far);

    Vec3 forward, up, target;
    fw64_transform_forward(&camera->transform, &forward);
    fw64_transform_up(&camera->transform, &up);

    vec3_add(&target, &camera->transform.position, &forward);
    matrix_camera_look_at(view, &camera->transform.position, &target, &up);
}

int fw64_camera_ray_from_window_pos(fw64Camera* camera, IVec2* window_pos, Vec3* ray_origin, Vec3* ray_direction) {
    float view[16], proj[16];
    _temp_camera_compute_view_projection(camera, view, proj);

    Vec3 viewport_pt = {window_pos->x, fw64_display_get_size(camera->display).y - window_pos->y, 0.0f}, far_pt;
    fw64_matrix_unproject(&viewport_pt, view, proj, &camera->viewport.position, &camera->viewport.size, ray_origin);

    viewport_pt.z = 1.0f;
    fw64_matrix_unproject(&viewport_pt, view, proj, &camera->viewport.position, &camera->viewport.size, &far_pt);

    vec3_subtract(ray_direction, &far_pt, ray_origin);
    vec3_normalize(ray_direction);

    return 1;
}

int fw64_camera_ray_from_viewport_pos(fw64Camera* camera, IVec2* viewport_pos, Vec3* ray_origin, Vec3* ray_direction) {
    IVec2 window_pos = *viewport_pos;
    ivec2_add(&window_pos, &window_pos, &camera->viewport.position);

    return fw64_camera_ray_from_window_pos(camera, &window_pos, ray_origin, ray_direction);
}

int fw64_camera_world_to_viewport_pos(fw64Camera* camera, Vec3* world_pos, IVec2* viewport_pos) {
    float view[16], proj[16];
    _temp_camera_compute_view_projection(camera, view, proj);
    Vec3 result;

    if (fw64_matrix_project(world_pos, view, proj, &camera->viewport.size, &result)) {
        viewport_pos->x = (int)result.x;
        viewport_pos->y = camera->viewport.size.y - (int)result.y;

        return 1;
    } else {
        return 0;
    }
}