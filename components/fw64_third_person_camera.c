#include "fw64_third_person_camera.h"

#define THIRD_PERSON_CAMERA_DEFAULT_DISTANCE 5.0f

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Engine* engine, fw64Camera* camera, fw64Transform* target) {
    cam->engine = engine;;
    cam->camera = camera;
    cam->target = target;

    cam->distance = THIRD_PERSON_CAMERA_DEFAULT_DISTANCE;
    
}

void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam) {
    Vec3 forward = {0.0f, 0.0f, -1.0f};
    vec3_scale(&forward, &forward, cam->distance);
    vec3_add(&cam->camera->node->transform.position, &cam->target->position, &forward);

    fw64_camera_update_view_matrix(cam->camera);
}
