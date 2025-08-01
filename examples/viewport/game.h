#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_rotate_node.h"

typedef enum {
    RENDER_PASS_VIEW,
    RENDER_PASS_INFO,
    RENDER_PASS_COUNT
}ViewportRenderPass;

typedef struct {
    fw64Engine* engine;
    fw64Font* font;

    fw64Camera camera;
    fw64Scene scene;
    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
    fw64SpriteBatch* spritebatch;

    int camera_pos_delta;
    int camera_size_delta;
    fw64RotateNode rotate_node;
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
