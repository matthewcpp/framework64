#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_rotate_node.h"

typedef enum {
    RENDER_PASS_SCENE,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} RenderPass;

typedef struct {
    uint64_t magic;
    uint64_t counter;
    uint64_t example_padding[62];
} SaveData;

typedef struct {
    SaveData save_data;
    fw64Engine* engine;
    fw64Font* font;
    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
    fw64SpriteBatch* spritebatch;
    fw64RotateNode rotate_node;
    fw64Scene scene;
    int loaded;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* example, fw64Engine* engine);
void game_fixed_update(Game* game);
void game_update(Game* example);
void game_draw(Game* example);

#ifdef __cplusplus
}
#endif
