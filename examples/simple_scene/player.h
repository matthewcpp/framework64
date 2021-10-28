#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/scene.h"


#define PLAYER_DEFAULT_ACCELERATION 9.5f
#define PLAYER_DEFAULT_DECELERATION 15.0f
#define PLAYER_DEFAULT_MAX_SPEED 10

#define PLAYER_DEFAULT_ROTATION_SPEED 90.0f

#define PLAYER_DEFAULT_JUMP_VELOCITY 10.0f
#define PLAYER_DEFAULT_GRAVITY (-20.0f)

#define PLAYER_STICK_THRESHOLD 0.15f

#define PLAYER_DEFAULT_HEIGHT 10.0f
#define PLAYER_DEFAULT_RADIUS 3.0f;

typedef enum {
    PLAYER_STATE_ON_GROUND,
    PLAYER_STATE_FALLING
} PlayerState;

typedef struct {
    fw64Node node;
    fw64Collider collider;
    fw64Engine* engine;
    fw64Scene* scene;

    Vec3 previous_position;

    float jump_impulse;
    float air_velocity;
    float gravity;

    float height;
    float radius;

    float speed;
    float acceleration;
    float deceleration;
    float max_speed;

    float rotation;
    float rotation_speed;
    PlayerState state;

    int process_input;
    int controller_num;

} Player;

#ifdef __cplusplus
extern "C" {
#endif

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, int mesh_index, Vec3* position);
void player_update(Player* player);
void player_draw(Player* player);

void player_reset(Player* player, Vec3* position);
void player_calculate_size(Player* player);

#ifdef __cplusplus
}
#endif