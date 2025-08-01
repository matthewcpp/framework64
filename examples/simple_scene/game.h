#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "player.h"
#include "chase_cam.h"
#include "ui.h"

typedef enum {
    RENDER_PASS_SCENE,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} RenderPass;

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    UI ui;

    Player player;
    ChaseCamera chase_cam;
    fw64Node* respawn_node;
    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_fixed_update(Game* game);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
