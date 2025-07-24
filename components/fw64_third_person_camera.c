#include "fw64_third_person_camera.h"

#include <framework64/math.h>

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Transform* target, fw64Camera* camera){
    cam->target = target;
    cam->camera = camera;
    fw64_third_person_camera_reset(cam);
}

static void fw64_third_person_camera_update_cam_pos(fw64ThirdPersonCamera* cam) {
    Vec3 forward = vec3_forward(), up = vec3_up(), look_at_target;

    // Calculate the camera's new offset position based on the rotation angles
    Quat q;
    quat_from_euler(&q, cam->rotation_x, cam->rotation_y, 0.0);

    quat_transform_vec3(&q, &up, &up);
    vec3_normalize(&up);

    quat_transform_vec3(&q, &forward, &forward);
    vec3_scale(&forward, cam->follow_dist, &forward);
    vec3_add(&forward, &cam->target_offset, &forward);
    vec3_add(&cam->target->position, &forward, &cam->camera->node->transform.position);

    vec3_add(&cam->target->position, &cam->target_offset, &look_at_target);
    fw64_transform_look_at(&cam->camera->node->transform, &look_at_target, &up);
    fw64_node_update(cam->camera->node);
    fw64_camera_update_view_matrix(cam->camera);
}

void fw64_third_person_camera_rotate(fw64ThirdPersonCamera* cam, float x, float y) {
    cam->rotation_x = fw64_clamp(cam->rotation_x + x, -90.0f, 90.0f);
    cam->rotation_y += y;
}

void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam) {
    fw64_third_person_camera_update_cam_pos(cam);
}

void fw64_third_person_camera_reset(fw64ThirdPersonCamera* cam) {
    cam->follow_dist = FW64_THIRD_PERSON_CAMERA_DEFAULT_FOLLOW_DISTANCE;
    cam->rotation_x = 0.0f; // temp pick default value
    vec3_set_zero(&cam->target_offset);

    Vec3 target_back, forward = vec3_forward();
    fw64_transform_back(cam->target, &target_back);
    cam->rotation_y = (atan2f(target_back.z, target_back.x) - atan2f(forward.z, forward.x)) * (180.0f / M_PI);
}
