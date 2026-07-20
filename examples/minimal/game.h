#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"
#include "framework64/util/bump_allocator.h"

#define KNIGHT_MAX 2

typedef struct {
    fw64Engine* engine;
    fw64BumpAllocator bump_allocator;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    fw64Texture* test_texture1;
    fw64Texture* test_texture2;
    fw64Texture* test_texture3;
    fw64Font* font;
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
