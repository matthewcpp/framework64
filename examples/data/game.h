#pragma once

#include "framework64/font.h"
#include "framework64/engine.h"

#include "framework64/n64/scene.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Font* consolas;
    char str_buff[64];
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
