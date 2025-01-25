#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"

#define KNIGHT_MAX 2

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    fw64Texture* test_texture;
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
