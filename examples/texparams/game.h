#ifndef GAME_H
#define GAME_H

#include "../common/entity.h"

#include "framework64/font.h"
#include "framework64/engine.h"

typedef enum {
    MODE_DEFAULT,
    MODE_CLAMP,
    MODE_WRAP,
    MODE_MIRROR
} Mode;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    Entity quad_entity;
    Mode mode;
    const char* mode_name;
    fw64Texture* buttons;
    fw64Texture* texture;
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

#endif