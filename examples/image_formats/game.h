#pragma once

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"

#include "fw64_ui_navigation.h"

typedef enum {
    IMAGE_FORMAT_NONE,
    IMAGE_FORMAT_RGBA32,
    IMAGE_FORMAT_RGBA16,
    IMAGE_FORMAT_CI8,
    IMAGE_FORMAT_CI4,
    IMAGE_FORMAT_IA8,
    IMAGE_FORMAT_IA4,
    IMAGE_FORMAT_I8,
    IMAGE_FORMAT_I4,
    IMAGE_FORMAT_COUNT
} ImageFormat;

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    fw64Texture* texture;
    ImageFormat image_format;
    const char* format_name;
    fw64Font* font;
    fw64BumpAllocator tex_allocator;
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
