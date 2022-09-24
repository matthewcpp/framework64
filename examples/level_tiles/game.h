#pragma once

#include "framework64/engine.h"
#include "framework64/level.h"
#include "framework64/util/bump_allocator.h"
#include "common/fps_camera.h"

#define ACTIVE_TILE_COUNT 6

typedef struct {
    fw64Engine* engine;
    fw64Level level;
    fw64FpsCamera fps_camera;
    fw64BumpAllocator allocators[ACTIVE_TILE_COUNT];
    uint32_t chunk_handles[ACTIVE_TILE_COUNT];
    float next_row_pos;
    float next_row_trigger;
    int handle_index;
    float player_previous_z;
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
