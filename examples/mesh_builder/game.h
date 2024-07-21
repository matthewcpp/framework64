#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "framework64/util/bump_allocator.h"

#include "fw64_ui_navigation.h"

typedef enum {
    GAME_STATE_NONE,

    GAME_STATE_TRIANGLES,
    GAME_STATE_QUADS,
    GAME_STATE_LINES,

    GAME_STATE_COUNT
} GameState;

typedef struct {
    fw64Engine* engine;
    GameState game_state;
    fw64RenderPass* renderpass;
    fw64Mesh* mesh;
    fw64Scene scene;
    fw64BumpAllocator allocator;
    fw64UiNavigation ui_nav;
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
