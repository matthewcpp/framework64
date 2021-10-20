#pragma once

#include "level1.h"
#include "level2.h"

#include "framework64/engine.h"

typedef union {
    Level1 level1;
    Level2 level2;
} Levels;

typedef struct {
    fw64Engine* engine;
    Levels levels;
    int level;
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
