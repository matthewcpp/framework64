#ifndef GAME_H
#define GAME_H

#include "framework64/font.h"
#include "framework64/system.h"

typedef enum {
    MODE_DEFAULT,
    MODE_CLAMP,
    MODE_WRAP,
    MODE_MIRROR
} Mode;

typedef struct {
    System* system;
    Camera camera;
    Entity quad_entity;
    Mode mode;
    const char* mode_name;
    ImageSprite* buttons;
    Font* font;
} Game;

void game_init(Game* game, System* system);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif