#include "ultra/camera.h"

void camera_init(Camera* camera) {
    transform_init(&camera->transform);

    camera->near = 0.1f;
    camera->far = 100.0f;
    camera->fovy = 45.0f;
    camera->aspect = 1.33f;
    camera->transform.position.z = 5.0f;

    camera_update_projection_matrix(camera);
    camera_update_view_matrix(camera);
}

void camera_update_projection_matrix(Camera* camera) {
    guPerspective(&camera->projection, &camera->perspNorm, 
    camera->fovy, camera->aspect, camera->near, camera->far, 1.0f);
}

void camera_update_view_matrix(Camera* camera) {
    Vec3 forward, up, target;
    transform_forward(&camera->transform, &forward);
    transform_up(&camera->transform, &up);
    vec3_normalize(&up);

    vec3_add(&target, &camera->transform.position, &forward);

    guLookAt(&camera->view, 
    camera->transform.position.x, camera->transform.position.y, camera->transform.position.z,
    target.x, target.y, target.z, up.x, up.y, up.z);
}