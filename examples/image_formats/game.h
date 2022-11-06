#pragma once

#include "framework64/engine.h"

typedef enum {
    IMAGE_FORMAT_NONE,
    IMAGE_FORMAT_IA8,
    IMAGE_FORMAT_IA4,
    IMAGE_FORMAT_RGBA32,
    IMAGE_FORMAT_COUNT
} ImageFormat;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Texture* texture;
    ImageFormat image_format;
    fw64Font* font;
    IVec2 header_size;
    IVec2 screen_size;
    char header_text[24];
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
