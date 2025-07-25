#pragma once

#include "player.h"
#include "ui.h"

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_collision_scene_manager.h"
#include "fw64_headlight.h"
#include "fw64_skybox.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    UI ui;
    fw64CharacterEnvironment character_environment;
    Player player;
    fw64CollisionSceneManager collision_scene_manager;
    fw64Headlight headlight;
    fw64Skybox skybox;
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
