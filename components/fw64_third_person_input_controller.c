#include "fw64_third_person_input_controller.h"

#include "framework64/controller_mapping/n64.h"

#define FW64_THIRD_PERSON_INPUT_STICK_THRESHOLD 0.1f
#define FW64_THIRD_PERSON_INPUT_CAM_DEFAULT_ROTATION_SPEED 60.0f
#define FW64_THIRD_PERSON_INPUT_CAM_DEFAULT_DISTANCE_SPEED 5.0f

void fw64_third_person_input_controller_init(fw64ThirdPersonInputController* controller, fw64Input* input, fw64Character* character, fw64Node* node, fw64ThirdPersonCamera* cam, fw64CharacterAnimationController* anim, int port) {
    controller->input = input;
    controller->character = character;
    controller->node = node;
    controller->cam = cam;
    controller->anim = anim;

    controller->port = port;
    controller->stick_threshold = FW64_THIRD_PERSON_INPUT_STICK_THRESHOLD;
    controller->cam_rotation_speed = FW64_THIRD_PERSON_INPUT_CAM_DEFAULT_ROTATION_SPEED;
    controller->cam_dist_speed = FW64_THIRD_PERSON_INPUT_CAM_DEFAULT_DISTANCE_SPEED;
}

static void fw64_third_person_input_controller_process_movement(fw64ThirdPersonInputController* controller) {
    Vec2 stick;
    fw64_input_controller_stick(controller->input, controller->port, &stick);

    int did_move = 0;
    Vec3 forward = vec3_zero(), right = vec3_zero();
    if (stick.y >= controller->stick_threshold || stick.y < -controller->stick_threshold) {
        fw64_transform_forward(&controller->cam->camera->node->transform, &forward);
        forward.y = 0.0f;
        vec3_normalize(&forward);
        vec3_scale(&forward, stick.y, &forward);
        did_move = 1;
    }

    if (stick.x >= controller->stick_threshold || stick.x < -controller->stick_threshold) {
        fw64_transform_right(&controller->cam->camera->node->transform, &right);
        right.y = 0.0f;
        vec3_normalize(&right);
        vec3_scale(&right, stick.x, &right);
        did_move = 1;
    }

    if (did_move) {
        vec3_add(&forward, &right, &controller->character->attempt_to_move);
        vec3_normalize(&controller->character->attempt_to_move);

        float target_yaw = atan2f(controller->character->attempt_to_move.x, controller->character->attempt_to_move.z);
        quat_set_axis_angle(&controller->node->transform.rotation, 0.0, 1.0, 0.0, target_yaw);
    }

    if (fw64_input_controller_button_pressed(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_A)) {
        controller->character->attempt_to_jump = 1;
    } else if (fw64_input_controller_button_pressed(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_B)) {
        if (fw64_character_animation_controller_state_is_idle(controller->anim) || fw64_character_animation_controller_state_is_running(controller->anim)) {
            fw64_character_animation_controller_start_primary_action(controller->anim);
        }
    }
}

static void _fw64_third_person_input_controller_process_ledge_input(fw64ThirdPersonInputController* controller) {
    if (fw64_character_is_climbing_up_ledge(controller->character)) {
        return;
    }

    Vec2 stick;
    fw64_input_controller_stick(controller->input, controller->port, &stick);

    if (stick.y < -0.5f) {
        fw64_character_drop_from_ledge(controller->character);
    } else if (fw64_input_controller_button_pressed(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_A)) {
       fw64_character_start_climbing_up_ledge(controller->character);
    }
}

void fw64_third_person_input_controller_update(fw64ThirdPersonInputController* controller, float time_delta) {
    if (!fw64_character_animation_controller_state_is_primary_action(controller->anim)) {
        if (fw64_character_is_interacting_with_ledge(controller->character)) {
            _fw64_third_person_input_controller_process_ledge_input(controller);
        } else {
            fw64_third_person_input_controller_process_movement(controller);
        }
    }

    if (fw64_input_controller_button_down(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        fw64_third_person_camera_rotate(controller->cam, 0.0f, controller->cam_rotation_speed * time_delta);
    } else if (fw64_input_controller_button_down(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        fw64_third_person_camera_rotate(controller->cam, 0.0f, -controller->cam_rotation_speed * time_delta);
    } else if (fw64_input_controller_button_down(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        fw64_third_person_camera_rotate(controller->cam, controller->cam_rotation_speed * time_delta, 0.0f);
    } else if (fw64_input_controller_button_down(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        fw64_third_person_camera_rotate(controller->cam, -controller->cam_rotation_speed * time_delta, 0.0f);
    } else if (fw64_input_controller_button_down(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
        controller->cam->follow_dist += controller->cam_dist_speed * time_delta;
    } else if (fw64_input_controller_button_down(controller->input, controller->port, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
        controller->cam->follow_dist -= controller->cam_dist_speed * time_delta;
    }
}
