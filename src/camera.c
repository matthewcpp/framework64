#include "framework64/camera.h"

#include "framework64/matrix.h"

#include <string.h>

void fw64_camera_init(fw64Camera* camera, fw64Display* display) {
    fw64_transform_init(&camera->transform);

    camera->near = 0.1f;
    camera->far = 500.0f;
    camera->fovy = 45.0f;
    camera->aspect = 1.33f;
    camera->transform.position.z = 5.0f;

    fw64_camera_update_projection_matrix(camera);
    fw64_camera_update_view_matrix(camera);

    Vec2 pos = {0.0f, 0.0f};
    IVec2 size_i = fw64_display_get_size(display);
    Vec2 size = {size_i.x, size_i.y};
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
    camera->_viewport.vp.vtrans[1] = ((short)camera->viewport_size.y * 2) + ((short)camera->viewport_size.y * 4);
    camera->_viewport.vp.vtrans[2] = G_MAXZ / 2;
    camera->_viewport.vp.vtrans[3] = 0;
}
#endif

void fw64_camera_set_viewport(fw64Camera* camera, Vec2* viewport_pos, Vec2* viewport_size) {
    camera->_viewport_pos = *viewport_pos;
    camera->_viewport_size = *viewport_size;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    fw64_n64_libultra_camera_update_viewport(camera);
#endif
}

void fw64_camera_set_viewport_screen_relative(fw64Camera* camera, IVec2* screen_size, Vec2* position, Vec2* size) {
    camera->_viewport_pos.x = screen_size->x * position->x;
    camera->_viewport_pos.y = screen_size->y * position->y;
    camera->_viewport_size.x = screen_size->x * size->x;
    camera->_viewport_size.y = screen_size->y * size->y;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    fw64_n64_libultra_camera_update_viewport(camera);
#endif
}

void fw64_camera_update_viewport(fw64Camera* camera) {

}