#include "fw64_fly_camera.h"

#include "framework64/controller_mapping/n64.h"

#define DEFAULT_MOVEMENT_SPEED 10.0f
#define DEFAULT_TURN_SPEED 90.0f
#define STICK_THRESHOLD 0.15

static void fw64_fly_camera_tilt(fw64FlyCamera* fly_cam, float time_delta);
static void fw64_fly_camera_move(fw64FlyCamera* fly_cam, float time_delta);
static void fw64_fly_camera_dolly(fw64FlyCamera* fly_cam, float time_delta);
static void fw64_fly_camera_strafe(fw64FlyCamera* fly_cam, float time_delta);

void fw64_fly_camera_init(fw64FlyCamera* fly_cam, fw64Input* input, fw64Camera* camera) {
    fly_cam->camera = camera;
    fly_cam->input = input;

    vec3_set_zero(&fly_cam->rotation);

    fly_cam->movement_speed = DEFAULT_MOVEMENT_SPEED;
    fly_cam->turn_speed = DEFAULT_TURN_SPEED;
    fly_cam->player_index = 0;
}

void fw64_fly_camera_update(fw64FlyCamera* fly_cam, float time_delta) {
    fw64_fly_camera_tilt(fly_cam, time_delta);
    fw64_fly_camera_move(fly_cam, time_delta);
    fw64_fly_camera_strafe(fly_cam, time_delta);
    fw64_fly_camera_dolly(fly_cam, time_delta);

    quat_from_euler(&fly_cam->camera->node->transform.rotation, fly_cam->rotation.x, fly_cam->rotation.y, 0.0f);
    fw64_camera_update_view_matrix(fly_cam->camera);
}

void fw64_fly_camera_tilt(fw64FlyCamera* fly_cam, float time_delta) {
    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        fly_cam->rotation.x += fly_cam->turn_speed * time_delta;
    }

    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        fly_cam->rotation.x -= fly_cam->turn_speed * time_delta;
    }
}

void fw64_fly_camera_move(fw64FlyCamera* fly_cam, float time_delta) {
    Vec2 stick;
    fw64_input_controller_stick(fly_cam->input, fly_cam->player_index, &stick);
    float speed = 0.0f;

    if (stick.y > STICK_THRESHOLD) {
        speed = stick.y * fly_cam->movement_speed * time_delta;
    }

    if (stick.y < -STICK_THRESHOLD) {
        speed = stick.y * fly_cam->movement_speed * time_delta;
    }

    if (stick.x > STICK_THRESHOLD) {
        fly_cam->rotation.y -= fly_cam->turn_speed * time_delta;
    }

    if (stick.x < -STICK_THRESHOLD) {
        fly_cam->rotation.y += fly_cam->turn_speed * time_delta;
    }

    fw64Transform* transform = &fly_cam->camera->node->transform;

    Vec3 forward;
    fw64_transform_forward(transform, &forward);
    vec3_add_and_scale(&transform->position, &forward, speed, &transform->position);
}

void fw64_fly_camera_strafe(fw64FlyCamera* fly_cam, float time_delta) {
    fw64Transform* transform = &fly_cam->camera->node->transform;

    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        Vec3 right;
        fw64_transform_right(transform, &right);
        vec3_add_and_scale(&transform->position, &right, fly_cam->movement_speed * time_delta, &transform->position);
    }
    else if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        Vec3 left;
        fw64_transform_left(transform, &left);
        vec3_add_and_scale(&transform->position, &left, fly_cam->movement_speed * time_delta, &transform->position);
    }
}

static void fw64_fly_camera_dolly(fw64FlyCamera* fly_cam, float time_delta) {
    
    Vec3 movement = vec3_zero();

    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        vec3_set_right(&movement);
    } else if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        vec3_set_left(&movement);
    } 
    
    if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_Z)) {
        if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
            vec3_set_forward(&movement);
        } else if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
            vec3_set_back(&movement);
        }
    } else {
        if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
            vec3_set_up(&movement);
        } else if (fw64_input_controller_button_down(fly_cam->input, fly_cam->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
            vec3_set_down(&movement);
        }
    }



    fw64Transform* transform = &fly_cam->camera->node->transform;
    vec3_add_and_scale(&transform->position, &movement, fly_cam->movement_speed * time_delta, &transform->position);
}
