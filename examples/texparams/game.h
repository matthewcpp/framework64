#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "framework64/util/bump_allocator.h"

#include "ui_navigation/ui_navigation.h"

typedef enum {
    RENDER_PASS_SCENE,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} RenderPass;

typedef enum {
    MODE_DEFAULT,
    MODE_CLAMP,
    MODE_WRAP,
    MODE_MIRROR,
    MODE_COUNT
} Mode;

typedef struct {
    fw64Engine* engine;
    int mode;
    fw64Font* font;
    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
    fw64SpriteBatch* spritebatch;
    fw64BumpAllocator mesh_allocator;
    fw64Scene scene;
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
