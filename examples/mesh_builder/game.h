#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/util/bump_allocator.h"

typedef enum {
    GAME_STATE_NONE,

    GAME_STATE_TRIANGLES,
    GAME_STATE_QUADS,
    GAME_STATE_LINES,

    GAME_STATE_COUNT
} GameState;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64BumpAllocator allocator;
    fw64Node node;
    GameState game_state;
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
