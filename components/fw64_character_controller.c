#include "fw64_character_controller.h"

#define FW64_CHARACTER_CONTROLLER_DEFAULT_ACCELERATION 9.5f
#define FW64_CHARACTER_CONTROLLER_DEFAULT_DECELERATION 15.0f
#define FW64_CHARACTER_CONTROLLER_DEFAULT_MAX_SPEED 10

#define FW64_CHARACTER_CONTROLLER_DEFAULT_ROTATION_SPEED 90.0f

#define FW64_CHARACTER_CONTROLLER_DEFAULT_JUMP_VELOCITY 10.0f
#define FW64_CHARACTER_CONTROLLER_DEFAULT_GRAVITY (-20.0f)

#define FW64_CHARACTER_CONTROLLER_DEFAULT_HEIGHT 10.0f
#define FW64_CHARACTER_CONTROLLER_DEFAULT_RADIUS 3.0f;

void fw64_character_controller_init(fw64CharacterController* controller, fw64Scene* scene, fw64Collider* collider) {
    controller->scene = scene;
    controller->collider = collider;
    vec3_zero(&controller->velocity);

    controller->flags = FW64_CHARACTER_CONTROLLER_FLAG_NONE;
    controller->jump_impulse = FW64_CHARACTER_CONTROLLER_DEFAULT_JUMP_VELOCITY;
    controller->gravity = FW64_CHARACTER_CONTROLLER_DEFAULT_GRAVITY;
    controller->max_speed = FW64_CHARACTER_CONTROLLER_DEFAULT_MAX_SPEED;
    controller->acceleration = FW64_CHARACTER_CONTROLLER_DEFAULT_ACCELERATION;
    controller->deceleration = FW64_CHARACTER_CONTROLLER_DEFAULT_DECELERATION;

    controller->height = FW64_CHARACTER_CONTROLLER_DEFAULT_HEIGHT;
    controller->radius = FW64_CHARACTER_CONTROLLER_DEFAULT_RADIUS;

    fw64_character_controller_reset(controller);
}

void fw64_character_controller_reset(fw64CharacterController* controller) {
    controller->speed = 0.0f;
    controller->rotation = 0.0f;
    controller->state = FW64_CHARACTER_CONTROLLER_STATE_GROUNDED;
    controller->air_velocity = 0.0f;
}

static Vec3 calculate_movement_vector(fw64CharacterController* controller, float time_delta) {
    quat_set_axis_angle(&controller->collider->node->transform.rotation, 0, 1, 0, controller->rotation * ((float)M_PI / 180.0f));

    Vec3 movement;
    fw64_transform_forward(&controller->collider->node->transform, &movement);
    vec3_scale(&movement, &movement, controller->speed * time_delta);

    movement.y += (controller->air_velocity * time_delta) + (controller->gravity * time_delta * time_delta / 2.0f);
    controller->air_velocity += controller->gravity * time_delta;

    return movement;
}

#define LAYER_DEFAULT 1U
#define LAYER_GROUND 2U
#define LAYER_WALL 4U

static void fw64_character_controller_update_speed(fw64CharacterController* controller, float time_delta) {
    if (controller->flags & FW64_CHARACTER_CONTROLLER_FLAG_ACCELERATE) {
        controller->speed = fminf(controller->speed + controller->acceleration * time_delta, controller->max_speed);
        controller->flags &= ~FW64_CHARACTER_CONTROLLER_FLAG_ACCELERATE;
    }
    else if (controller->flags & FW64_CHARACTER_CONTROLLER_FLAG_DECELERATE) {
        controller->speed = fmaxf(controller->speed - controller->acceleration * time_delta, -controller->max_speed);
        controller->flags &= ~FW64_CHARACTER_CONTROLLER_FLAG_DECELERATE;
    }
    else {
        float decel = controller->deceleration * time_delta;
        if (controller->speed > 0.0f)
            controller->speed = fmaxf(controller->speed - decel, 0.0f);
        else if (controller->speed < 0.0f)
            controller->speed = fminf(controller->speed + decel, 0.0f);
    }
}

static void fw64_character_controller_update_position(fw64CharacterController* controller, float time_delta) {
    Vec3* position = &controller->collider->node->transform.position;
    controller->previous_position = *position;

    fw64_character_controller_update_speed(controller, time_delta);
    Vec3 movement = calculate_movement_vector(controller, time_delta);
    vec3_add(position, position, &movement);

    fw64CharacterControllerState new_state = FW64_CHARACTER_CONTROLLER_STATE_FALLING;
    float height_radius = controller->height / 2.0f;
    Vec3 query_center = *position;
    query_center.y += height_radius;

    fw64OverlapSphereQueryResult result;
    if (fw64_scene_partition_overlap_sphere(controller->scene->partition, &query_center, height_radius, 0xFFFFFFFF, &result)) {
        for (int i = 0; i < result.count; i++) {
            fw64OverlapSphereResult *hit = result.results + i;

            Vec3 direction;
            vec3_subtract(&direction, &query_center, &hit->point);
            vec3_normalize(&direction);

            int is_grounded = (hit->node->layer_mask & LAYER_GROUND) || direction.y > 0.9f;

            // ground
            if (is_grounded && controller->air_velocity <= 0.0f) {
                new_state = FW64_CHARACTER_CONTROLLER_STATE_GROUNDED;
                controller->air_velocity = 0;
                float hit_distance = vec3_distance(&query_center, &hit->point);
                vec3_add_and_scale(position, position, &direction, height_radius - hit_distance);
            } else { // walls
                float hit_distance = vec3_distance(&query_center, &hit->point);

                if (hit_distance < controller->radius) {
                    vec3_add_and_scale(position, position, &direction, controller->radius - hit_distance);
                }
            }
        }
    }

    controller->state = new_state;

    fw64_node_update(controller->collider->node);
}

void fw64_character_controller_fixed_update(fw64CharacterController* controller, float time_delta) {
    // Vec3 center, extents, delta;
    // box_center(&controller->collider->bounding, &center);
    // box_extents(&controller->collider->bounding, &extents);

    // vec3_scale(&delta, &controller->velocity, time_delta);
    // vec3_add(&center, &center, &delta);
    // Box swept;
    // box_set_center_extents(&swept, &center, &extents);
    // box_encapsulate_box(&swept, &controller->collider->bounding);

    fw64_character_controller_update_position(controller, time_delta); 
}

void fw64_character_controller_accelerate(fw64CharacterController* controller) {
    controller->flags |= FW64_CHARACTER_CONTROLLER_FLAG_ACCELERATE;
}

void fw64_character_controller_decelerate(fw64CharacterController* controller) {
    controller->flags |= FW64_CHARACTER_CONTROLLER_FLAG_DECELERATE;
}

void fw64_character_controller_jump(fw64CharacterController* controller) {
    controller->air_velocity = controller->jump_impulse;
}
