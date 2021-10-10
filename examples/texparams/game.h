#pragma once

#include "framework64/font.h"
#include "framework64/engine.h"
#include "framework64/node.h"

typedef enum {
    MODE_DEFAULT,
    MODE_CLAMP,
    MODE_WRAP,
    MODE_MIRROR
} Mode;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Node quad;
    Mode mode;
    const char* mode_name;
    fw64Texture* buttons;
    fw64Font* font;
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
