#include "framework64/camera.h"

void fw64_camera_init(fw64Camera* camera) {
    fw64_transform_init(&camera->transform);

    camera->near = 0.1f;
    camera->far = 1000.0f;
    camera->fovy = 45.0f;
    camera->aspect = 1.33f;
    camera->transform.position.z = 5.0f;

    fw64_camera_update_projection_matrix(camera);
    fw64_camera_update_view_matrix(camera);
}

void fw64_camera_update_projection_matrix(fw64Camera* camera) {
    guPerspective(&camera->projection, &camera->perspNorm, 
    camera->fovy, camera->aspect, camera->near, camera->far, 1.0f);
}

void fw64_camera_update_view_matrix(fw64Camera* camera) {
    Vec3 forward, up, target;
    fw64_transform_forward(&camera->transform, &forward);
    fw64_transform_up(&camera->transform, &up);
    vec3_normalize(&up);

    vec3_add(&target, &camera->transform.position, &forward);

    guLookAt(&camera->view, 
    camera->transform.position.x, camera->transform.position.y, camera->transform.position.z,
    target.x, target.y, target.z, up.x, up.y, up.z);
}