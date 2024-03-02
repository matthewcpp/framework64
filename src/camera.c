#include "framework64/camera.h"

#include "framework64/matrix.h"
#include "framework64/matrix_project.h"

#include <string.h>

void fw64_camera_init(fw64Camera* camera, fw64Display* display) {
    fw64_transform_init(&camera->transform);
    camera->display = display;

    camera->near = 0.1f;
    camera->far = 500.0f;
    camera->fovy = 45.0f;
    camera->aspect = 1.33f;
    camera->transform.position.z = 5.0f;

    fw64_camera_update_projection_matrix(camera);
    fw64_camera_update_view_matrix(camera);

    IVec2 pos = {0.0f, 0.0f};
    IVec2 size = fw64_display_get_size(display);
    fw64_camera_set_viewport(camera, &pos, &size);
}

void fw64_camera_update_projection_matrix(fw64Camera* camera) {
#ifdef FW64_PLATFORM_N64_LIBULTRA
    guPerspective(&camera->projection, &camera->_persp_norm, 
    camera->fovy, camera->aspect, camera->near, camera->far, 1.0f);
#else
    matrix_perspective(&camera->projection.m[0], camera->fovy, camera->aspect, camera->near, camera->far);
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

#ifdef FW64_PLATFORM_N64_LIBULTRA
void fw64_n64_libultra_camera_update_viewport(fw64Camera* camera) {
    camera->_viewport.vp.vscale[0] = (short)camera->viewport_size.x * 2;
    camera->_viewport.vp.vscale[1] = (short)camera->viewport_size.y * 2;
    camera->_viewport.vp.vscale[2] = G_MAXZ / 2;
    camera->_viewport.vp.vscale[3] = 0;

    camera->_viewport.vp.vtrans[0] = ((short)camera->viewport_size.x * 2) + ((short)camera->viewport_pos.x * 4);
    camera->_viewport.vp.vtrans[1] = ((short)camera->viewport_size.y * 2) + ((short)camera->viewport_pos.y * 4);
    camera->_viewport.vp.vtrans[2] = G_MAXZ / 2;
    camera->_viewport.vp.vtrans[3] = 0;
}
#endif

void fw64_camera_set_viewport(fw64Camera* camera, IVec2* viewport_pos, IVec2* viewport_size) {
    camera->viewport_pos = *viewport_pos;
    camera->viewport_size = *viewport_size;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    fw64_n64_libultra_camera_update_viewport(camera);
#endif
}

void fw64_camera_set_viewport_relative(fw64Camera* camera, fw64Display* display, Vec2* viewport_position, Vec2* viewport_size) {
    IVec2 screen_size = fw64_display_get_size(display);

    camera->viewport_pos.x = screen_size.x * viewport_position->x;
    camera->viewport_pos.y = screen_size.y * viewport_position->y;
    camera->viewport_size.x = screen_size.x * viewport_size->x;
    camera->viewport_size.y = screen_size.y * viewport_size->y;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    fw64_n64_libultra_camera_update_viewport(camera);
#endif
}

int fw64_camera_ray_from_window_pos(fw64Camera* camera, IVec2* window_pos, Vec3* ray_origin, Vec3* ray_direction) {
    // p
    Vec3 viewport_pt = {window_pos->x, fw64_display_get_size(camera->display).y - window_pos->y, 0.0f}, far_pt;
    float view[16], proj[16];

    matrix_perspective(proj, camera->fovy, camera->aspect, camera->near, camera->far);

    Vec3 forward, up, target;
    fw64_transform_forward(&camera->transform, &forward);
    fw64_transform_up(&camera->transform, &up);

    vec3_add(&target, &camera->transform.position, &forward);
    matrix_camera_look_at(view, &camera->transform.position, &target, &up);

    fw64_matrix_unproject(&viewport_pt, view, proj, &camera->viewport_pos, &camera->viewport_size, ray_origin);
    viewport_pt.z = 1.0f;
    fw64_matrix_unproject(&viewport_pt, view, proj, &camera->viewport_pos, &camera->viewport_size, &far_pt);

    vec3_subtract(ray_direction, &far_pt, ray_origin);
    vec3_normalize(ray_direction);

    return 1;
}

int fw64_camera_ray_from_viewport_pos(fw64Camera* camera, IVec2* viewport_pos, Vec3* ray_origin, Vec3* ray_direction) {
    IVec2 window_pos = *viewport_pos;
    ivec2_add(&window_pos, &window_pos, &camera->viewport_pos);

    return fw64_camera_ray_from_window_pos(camera, &window_pos, ray_origin, ray_direction);
}