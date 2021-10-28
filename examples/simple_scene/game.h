#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "player.h"
#include "chase_cam.h"
#include "ui.h"

typedef struct {
    fw64Engine* engine;

    fw64Scene* scene;
    UI ui;

    Player player;
    ChaseCamera chase_cam;
    fw64Node* respawn_node;
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
