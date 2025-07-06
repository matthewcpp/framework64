#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"

typedef enum {
    WEAPON_SWORD,
    WEAPON_MACE
} Weapon;

typedef enum {
    SHIELD_KITE,
    SHIELD_BUCKLER
} Shield;

// For simplicity's sake, all categories i.e. Weapons / Shields should have same count
#define SELECTION_CATEGORY_COUNT 2

typedef enum {
    SELECT_WEAPON,
    SELECT_SHIELD,
    SELECT_COUNT
} Selection;

typedef enum {
    RENDERPASS_SCENE,
    RENDERPASS_UI,
    RENDERPASS_COUNT
} Renderpass;

typedef struct {
    fw64Engine* engine;

    fw64Scene scene;

    fw64Mesh* weapons[SELECTION_CATEGORY_COUNT];
    fw64Mesh* shields[SELECTION_CATEGORY_COUNT];
    fw64Node* select_nodes[SELECT_COUNT];
    int selections[SELECT_COUNT];
    int select_type;

    fw64Camera camera;
    fw64ArcballCamera arcball;

    fw64Font* font;

    fw64SpriteBatch* spritebatch;
    fw64RenderPass* renderpass[RENDERPASS_COUNT];

    fw64SkinnedMeshInstance* character;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
