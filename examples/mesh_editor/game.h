#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/util/bump_allocator.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64BumpAllocator bump_allocator;
    fw64Mesh* mesh;
    fw64Node node;
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
