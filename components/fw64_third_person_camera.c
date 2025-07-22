#include "fw64_third_person_camera.h"

#include <framework64/math.h>

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Transform* target, fw64Camera* camera){
    cam->target = target;
    cam->camera = camera;
    fw64_third_person_camera_reset(cam);
}

static void fw64_third_person_camera_update_cam_pos(fw64ThirdPersonCamera* cam) {
    Vec3 target_back, forward = vec3_forward(), up = vec3_up();
    fw64_transform_back(cam->target, &target_back); // TODO: flip this?

    Quat q;
    quat_from_euler(&q, 0.0f, cam->rotation_y, cam->rotation_z);
    quat_transform_vec3(&q, &forward, &forward);
    vec3_scale(&forward, cam->follow_dist, &forward);
    forward.y = cam->offset_height;

    vec3_add(&cam->target->position, &forward, &cam->camera->node->transform.position);
    fw64_transform_look_at(&cam->camera->node->transform, &cam->target->position, &up);
    fw64_node_update(cam->camera->node);
    fw64_camera_update_view_matrix(cam->camera);
}

void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam) {
    fw64_third_person_camera_update_cam_pos(cam);
}

void fw64_third_person_camera_reset(fw64ThirdPersonCamera* cam) {
    cam->follow_dist = FW64_THIRD_PERSON_CAMERA_DEFAULT_FOLLOW_DISTANCE;
    cam->rotation_z = 0.0f; // temp pick default value
    cam->offset_height = 12.5f;

    Vec3 target_back, forward = vec3_forward();
    fw64_transform_back(cam->target, &target_back); // TODO: flip this?
    cam->rotation_y = (atan2f(target_back.z, target_back.x) - atan2f(forward.z, forward.x)) * (180.0 / M_PI);

    fw64_third_person_camera_update_cam_pos(cam);
}
