#include "framework64/camera.h"

#include "framework64/matrix.h"

#include <string.h>

void fw64_camera_init(fw64Camera* camera) {
    fw64_transform_init(&camera->transform);

    camera->near = 0.1f;
    camera->far = 1000.0f;
    camera->fovy = 45.0f;
    camera->aspect = 1.33f;
    camera->transform.position.z = 5.0f;

    vec2_set(&camera->viewport_pos, 0.0f, 0.0f);
    vec2_set(&camera->viewport_size, 1.0f, 1.0f);

    fw64_camera_update_projection_matrix(camera);
    fw64_camera_update_view_matrix(camera);
}

void fw64_camera_update_projection_matrix(fw64Camera* camera) {
#ifdef PLATFORM_N64
    guPerspective(&camera->projection, &camera->perspNorm, 
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

#ifdef PLATFORM_N64
    guLookAt(&camera->view, 
    camera->transform.position.x, camera->transform.position.y, camera->transform.position.z,
    target.x, target.y, target.z, up.x, up.y, up.z);
#else
    matrix_camera_look_at(&camera->view.m[0], &camera->transform.position, &target, &up);
#endif
}

void fw64_camera_extract_frustum_planes(fw64Camera* camera, fw64Frustum* frustum) {
    float projScreenMatrix[16];

    #ifdef PLATFORM_N64
    Mtx projViewMatrix;
    guMtxCatL(&camera->view, &camera->projection, &projViewMatrix);
    guMtxL2F((void*)&projScreenMatrix[0], &projViewMatrix);
    #else
    matrix_multiply(&projScreenMatrix[0], &camera->projection.m[0], &camera->view.m[0]);
    #endif

    fw64_frustum_set_from_matrix(frustum, &projScreenMatrix[0]);
}
