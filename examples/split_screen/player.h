#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/scene.h"
#include "framework64/util/fps_camera.h"

typedef struct {
    fw64Mesh* mesh;
    Vec3 position;
    Quat rotation;
    Vec3 scale;
} Weapon;

typedef struct {
    fw64Engine* engine;
    fw64FpsCamera fps_camera;
    fw64Scene* scene;
    int player_index;
    int active;

    fw64Node* node;
    Weapon* current_weapon;
    fw64Camera weapon_camera;
    fw64Transform weapon_transform;
} Player;

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, int node_id, int index);
void player_update(Player* player);
void player_draw_view(Player* player);

void player_set_weapon(Player* player, Weapon* weapon);
void player_set_viewport_rect(Player* player, float x, float y, float w, float h);
