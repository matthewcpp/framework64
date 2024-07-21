#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/scene.h"
#include "fw64_fps_camera.h"

typedef struct {
    fw64Mesh* mesh;
    float projection_matrix[16];
    float view_matrix[16];
} Weapon;

typedef struct {
    fw64Engine* engine;
    fw64FpsCamera fps_camera;
    fw64Scene* scene;
    int player_index;

    fw64Node* node;
    fw64Node* weapon_node;
    
    Weapon* weapon;
    fw64RenderPass* scene_renderpass;
    fw64RenderPass* weapon_renderpass;
} Player;

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, int node_id, int index);
void player_update(Player* player);
void player_draw_view(Player* player);
void player_draw_weapon(Player* player);

void player_set_weapon(Player* player, Weapon* weapon);
void player_set_viewport_rect(Player* player, float x, float y, float w, float h);
