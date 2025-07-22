#include "fw64_third_person_camera.h"

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Transform* target, fw64Camera* camera){
    cam->target = target;
    cam->camera = camera;
    cam->follow_dist = FW64_THIRD_PERSON_CAMERA_DEFAULT_FOLLOW_DISTANCE;
    cam->offset_height = 12.5f;

    fw64_third_person_camera_reset(cam);
}

void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam) {
    Vec3 up, offset;

    // todo: look into fixing this
    fw64_transform_forward(&cam->camera->node->transform, &offset);
    offset.y = 0.0f;
    vec3_scale(&offset, cam->follow_dist, &offset);
    offset.y -= cam->offset_height;

    vec3_subtract(&cam->target->position, &offset, &cam->camera->node->transform.position);
    
    //vec3_set(&up, 0.0f, 1.0f, 0.0f);
    fw64_transform_up(&cam->camera->node->transform, &up);
    fw64_transform_look_at(&cam->camera->node->transform, &cam->target->position, &up);
    fw64_node_update(cam->camera->node);
    fw64_camera_update_view_matrix(cam->camera);
}

void fw64_third_person_camera_reset(fw64ThirdPersonCamera* cam) {
    Vec3 up, offset;
    fw64_transform_forward(cam->target, &offset);
    offset.y = 0.0f;
    vec3_scale(&offset, cam->follow_dist, &offset);
    offset.y += cam->offset_height;

    vec3_add(&cam->target->position, &offset, &cam->camera->node->transform.position);

    fw64_transform_up(&cam->camera->node->transform, &up);
    fw64_transform_look_at(&cam->camera->node->transform, &cam->target->position, &up);
}
