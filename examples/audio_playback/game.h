#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "framework64/sprite_batch.h"

#include "fw64_rotate_node.h"

typedef struct {
    fw64Scene scene;
    fw64Camera camera;
    fw64RotateNode rotate_node;
    fw64Engine* engine;
    fw64RenderPass* renderpass;
} SceneView;



typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* sprite_batch;
} UI;

typedef struct {
    fw64Engine* engine;
    SceneView scene_view;
    UI ui;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game *game, fw64Engine *engine);
void game_update(Game *game);
void game_draw(Game *game);

#ifdef __cplusplus
}
#endif
