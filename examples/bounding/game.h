#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "debug_boxes/debug_boxes.h"

typedef enum {
    RENDERPASS_SCENE_TRIANGLES,
    RENDERPASS_SCENE_LINES,
    RENDERPASS_COUNT
} RenderPass;

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    fw64Node* node;
    fw64Node* active;
} Penguin;

typedef struct {
    fw64Engine* engine;
    Penguin* penguin;
    fw64Scene* scene;
    fw64Font* font;
    fw64SpriteBatch* spritebatch;
    fw64RenderPass* renderpass;
    char text[32];
} Ui;

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    fw64Camera camera;
    fw64RenderPass* renderpass[RENDERPASS_COUNT];
    Penguin penguin;
    fw64DebugBoxes debug_boxes;
    Ui ui;
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
