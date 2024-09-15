#include "chase_cam.h"

#include "framework64/controller_mapping/n64.h"

#include <stddef.h>

#define CAMERA_FOLLOW_DIST 9.0f
#define CAMERA_FOLLOW_HEIGHT 10.0f
#define CAMERA_FORWARD_TARGET_DIST 5.0f
#define CAMERA_FORWARD_TARGET_HEIGHT 0.0f
#define CAMERA_ADJUST_SPEED 20.0f;

void chase_camera_init(ChaseCamera* chase_cam, fw64Engine* engine, fw64Scene* scene) {
    fw64Node* camera_node = fw64_scene_create_node(scene);
    fw64_camera_init(&chase_cam->camera, camera_node, fw64_displays_get_primary(engine->displays));
    chase_cam->camera.near = 1.0f;
    chase_cam->camera.far = 100.0f;
    fw64_camera_update_projection_matrix(&chase_cam->camera);

    chase_cam->engine = engine;
    chase_cam->target = NULL;

    chase_cam->target_follow_dist = CAMERA_FOLLOW_DIST;
    chase_cam->target_follow_height = CAMERA_FOLLOW_HEIGHT;
    chase_cam->target_forward_dist = CAMERA_FORWARD_TARGET_DIST;
    chase_cam->target_forward_height = CAMERA_FORWARD_TARGET_HEIGHT;
    chase_cam->camera_adjust_speed = CAMERA_ADJUST_SPEED;
}

static void chase_camera_update_position(ChaseCamera* chase_cam) {
    if (!chase_cam->target) return;

    Vec3 back, forward, camera_pos, camera_target;
    Vec3 up = {0.0f, 1.0f, 0.0f};
    fw64_transform_back(chase_cam->target, &back);
    vec3_copy(&forward, &back);
    vec3_negate(&forward);

    vec3_scale(&back, &back, chase_cam->target_follow_dist);
    vec3_add(&camera_pos, &chase_cam->target->position, &back);
    camera_pos.y += chase_cam->target_follow_height;

    vec3_scale(&forward, &forward, chase_cam->target_forward_dist);
    vec3_add(&camera_target, &chase_cam->target->position, &forward);
    forward.y += chase_cam->target_forward_height;

    chase_cam->camera.node->transform.position = camera_pos;
    fw64_transform_look_at(&chase_cam->camera.node->transform, &camera_target, &up);
    fw64_camera_update_view_matrix(&chase_cam->camera);
}

static void chase_camera_update_input(ChaseCamera* chase_cam) {
    if (fw64_input_controller_button_down(chase_cam->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_R)) {
        chase_cam->target_follow_height += chase_cam->camera_adjust_speed * chase_cam->engine->time->time_delta;
    }

    if (fw64_input_controller_button_down(chase_cam->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_L)) {
        chase_cam->target_follow_height -= chase_cam->camera_adjust_speed * chase_cam->engine->time->time_delta;
    }
}

void chase_camera_update(ChaseCamera* chase_cam) {
    chase_camera_update_position(chase_cam);
    chase_camera_update_input(chase_cam);
}
