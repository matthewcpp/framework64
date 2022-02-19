#pragma once

#include "framework64/engine.h"
#include "framework64/mesh.h"
#include "framework64/scene.h"

#include "player.h"

typedef struct {
    fw64Engine* engine;
    Player players[4];
    fw64Mesh* weapon_model;
    Weapon us_ar33;
    fw64Scene* scene;
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
