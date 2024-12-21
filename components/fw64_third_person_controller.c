#include "fw64_third_person_controller.h"

#include "framework64/controller_mapping/n64.h"

#define PLAYER_STICK_THRESHOLD 0.15f
#define PLAYER_DEFAULT_ROTATION_SPEED 90.0f

void fw64_third_person_controller_init(fw64ThirdPersonController* controller, fw64Engine* engine, fw64Scene* scene, fw64Collider* collider, fw64Camera* camera) {
    controller->engine = engine;
    controller->process_input = 1;
    controller->controller_num = 0;

    fw64_character_controller_init(&controller->base, scene, collider);
    fw64_third_person_camera_init(&controller->camera, engine, camera, &collider->node->transform);
}

static void process_input(fw64ThirdPersonController* controller) {
    Vec2 stick;
    fw64_input_controller_stick(controller->engine->input, 0, &stick);
    if (stick.x >= PLAYER_STICK_THRESHOLD || stick.x <= -PLAYER_STICK_THRESHOLD) {
        float rotation_delta = PLAYER_DEFAULT_ROTATION_SPEED * controller->engine->time->time_delta;

        if (stick.x >= PLAYER_STICK_THRESHOLD) {
            controller->base.rotation -= rotation_delta;
        }
        else if (stick.x <= -PLAYER_STICK_THRESHOLD) {
            controller->base.rotation += rotation_delta;
        }
    }

    if (stick.y >= PLAYER_STICK_THRESHOLD ) {
        fw64_character_controller_accelerate(&controller->base);
    }
    else if (stick.y <= -PLAYER_STICK_THRESHOLD) {
        fw64_character_controller_decelerate(&controller->base);
    }

    if (fw64_input_controller_button_pressed(controller->engine->input, controller->controller_num, FW64_N64_CONTROLLER_BUTTON_A)) {
        if (controller->base.state == FW64_CHARACTER_CONTROLLER_STATE_GROUNDED) {
            fw64_character_controller_jump(&controller->base);
        }
    }
}

void fw64_third_person_controller_fixed_update(fw64ThirdPersonController* controller) {
    fw64_character_controller_fixed_update(&controller->base, controller->engine->time->time_delta);
}

void fw64_third_person_controller_update(fw64ThirdPersonController* controller) {
    if (controller->process_input) {
        process_input(controller);
    }

    fw64_third_person_camera_update(&controller->camera);
}
