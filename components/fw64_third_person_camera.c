#include "fw64_third_person_camera.h"

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Transform* target, fw64Camera* camera){
    cam->target = target;
    cam->camera = camera;

    vec3_subtract(&camera->node->transform.position, &target->position, &cam->offset_vec);
}

void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam) {
    vec3_add(&cam->target->position, &cam->offset_vec, &cam->camera->node->transform.position);
    Vec3 up;
    fw64_transform_up(&cam->camera->node->transform, &up);
    fw64_transform_look_at(&cam->camera->node->transform, &cam->target->position, &up);
    fw64_node_update(cam->camera->node);
    fw64_camera_update_view_matrix(cam->camera);
}
