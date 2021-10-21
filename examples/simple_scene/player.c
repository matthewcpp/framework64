#include "player.h"

#include "framework64/n64/controller_button.h"

static void process_input(Player* player);
static void update_position(Player* player);
static void set_state(Player* player, PlayerState new_state);

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, int mesh_index, Vec3* position) {
    player->engine = engine;
    player->scene = scene;

    player->jump_impulse = PLAYER_DEFAULT_JUMP_VELOCITY;
    player->gravity = PLAYER_DEFAULT_GRAVITY;
    player->max_speed = PLAYER_DEFAULT_MAX_SPEED;
    player->acceleration = PLAYER_DEFAULT_ACCELERATION;
    player->deceleration = PLAYER_DEFAULT_DECELERATION;

    player->process_input = 1;
    player->controller_num = 0;

    fw64_node_init(&player->node, fw64_mesh_load(engine->assets, mesh_index));

    player_reset(player, position);
}

void player_reset(Player* player, Vec3* position) {
    player->speed = 0.0f;
    player->rotation = 0.0f;
    player->state = PLAYER_STATE_ON_GROUND;
    player->air_velocity = 0.0f;

    quat_ident(&player->node.transform.rotation);

    if (position) {
        player->node.transform.position = *position;
        fw64_node_refresh(&player->node);
    }
}

void player_update(Player* player) {
    if (player->process_input)
        process_input(player);

    update_position(player);
}

void process_input(Player* player) {
    Vec2 stick;
    fw64_input_stick(player->engine->input, 0, &stick);
    if (stick.x >= PLAYER_STICK_THRESHOLD || stick.x <= -PLAYER_STICK_THRESHOLD) {
        float rotation_delta = PLAYER_DEFAULT_ROTATION_SPEED * player->engine->time->time_delta;

        if (stick.x >= PLAYER_STICK_THRESHOLD) {
            player->rotation -= rotation_delta;
        }
        else if (stick.x <= -PLAYER_STICK_THRESHOLD) {
            player->rotation += rotation_delta;
        }

        quat_set_axis_angle(&player->node.transform.rotation, 0, 1, 0, player->rotation * ((float)M_PI / 180.0f));
    }

    if (stick.y >= PLAYER_STICK_THRESHOLD ) {
        player->speed = fminf(player->speed + player->acceleration * player->engine->time->time_delta, player->max_speed);
    }
    else if (stick.y <= -PLAYER_STICK_THRESHOLD) {
        player->speed = fmaxf(player->speed - player->acceleration * player->engine->time->time_delta, -player->max_speed);
    }
    else {
        float decel = player->deceleration * player->engine->time->time_delta;
        if (player->speed > 0.0f)
            player->speed = fmaxf(player->speed - decel, 0.0f);
        else if (player->speed < 0.0f)
            player->speed = fminf(player->speed + decel, 0.0f);
    }

    if (fw64_input_button_pressed(player->engine->input, player->controller_num, FW64_N64_CONTROLLER_BUTTON_A)) {
        if (player->state == PLAYER_STATE_ON_GROUND) {
            player->air_velocity = player->jump_impulse;
        }
    }
}

static Vec3 calculate_movement_vector(Player* player) {
    float time_delta = player->engine->time->time_delta;

    Vec3 movement;
    fw64_transform_forward(&player->node.transform, &movement);
    vec3_scale(&movement, &movement, player->speed * time_delta);

    movement.y += (player->air_velocity * time_delta) + (player->gravity * time_delta * time_delta / 2.0f);
    player->air_velocity += player->gravity * time_delta;

    return movement;
}

#define LAYER_GROUND 2

static void check_floor(Player* player) {
    fw64RaycastHit hit;
}

void update_position(Player* player) {
    Vec3* position = &player->node.transform.position;

    Vec3 movement = calculate_movement_vector(player);

    Vec3 center;
    float radius;
    fw64_node_sphere(&player->node, &center, &radius);
    vec3_add(&center, &center, &movement);
    vec3_add(position, position, &movement);

    PlayerState new_state = PLAYER_STATE_FALLING;

    fw64OverlapSphereQueryResult result;
    if (fw64_scene_overlap_sphere(player->scene, &center, radius, 0xFFFFFFFF, &result)) {
        for (int i = 0; i < result.count; i++) {
            fw64OverlapSphereResult *hit = result.results + i;

            Vec3 direction;
            vec3_subtract(&direction, &center, &hit->point);
            vec3_normalize(&direction);

            // ground
            if (direction.y > 0.90f && player->air_velocity <= 0.0f) {
                new_state = PLAYER_STATE_ON_GROUND;
                player->air_velocity = 0;
                player->node.transform.position.y = hit->point.y;
            } else { // walls
                float hit_distance = vec3_distance(&center, &hit->point);

                Vec3 extents;
                box_extents(&player->node.bounding, &extents);

                float test_radius = direction.x > direction.z ? extents.x : extents.z;

                if (hit_distance < test_radius) {
                    float test_direction = direction.x > direction.z ? direction.x : direction.z;
                    vec3_add_and_scale(position, position, &direction, test_radius - hit_distance);
                }
            }
        }
    }

    set_state(player, new_state);

    fw64_node_refresh(&player->node);
}

static void set_state(Player* player, PlayerState new_state) {
    if (new_state == player->state)
        return;

    player->state = new_state;
}

void player_draw(Player* player) {
    fw64_renderer_draw_static_mesh(player->engine->renderer, &player->node.transform, player->node.mesh);
}