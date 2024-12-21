#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/scene.h"

#include "fw64_third_person_controller.h"


#define PLAYER_DEFAULT_ACCELERATION 9.5f
#define PLAYER_DEFAULT_DECELERATION 15.0f
#define PLAYER_DEFAULT_MAX_SPEED 10

#define PLAYER_DEFAULT_ROTATION_SPEED 90.0f

#define PLAYER_DEFAULT_JUMP_VELOCITY 10.0f
#define PLAYER_DEFAULT_GRAVITY (-20.0f)



#define PLAYER_DEFAULT_HEIGHT 10.0f
#define PLAYER_DEFAULT_RADIUS 3.0f;

typedef enum {
    PLAYER_STATE_ON_GROUND,
    PLAYER_STATE_FALLING
} PlayerState;

typedef struct {
    fw64Node* node;
    fw64Collider* collider;
    fw64Engine* engine;
    fw64Scene* scene;

    fw64ThirdPersonController character_controller;
} Player;

#ifdef __cplusplus
extern "C" {
#endif

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, fw64Camera* camera, fw64AssetId mesh_index, Vec3* position);
void player_fixed_update(Player* player);
void player_update(Player* player);

void player_reset(Player* player, Vec3* position);
void player_calculate_size(Player* player);

#ifdef __cplusplus
}
#endif
