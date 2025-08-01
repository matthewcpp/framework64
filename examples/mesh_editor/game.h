#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "framework64/util/bump_allocator.h"

#include "fw64_ui_navigation.h"

typedef struct {
    fw64Engine* engine;
    int tile_index;
    fw64Scene scene;
    fw64RenderPass* renderpass;
    fw64UiNavigation ui_nav;
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
