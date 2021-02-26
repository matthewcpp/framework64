#ifndef GAME_H
#define GAME_H

#include "framework64/system.h"

typedef struct {
    System* system;
    Camera camera;
    ImageSprite* buttons;
    Font* font;
    int sound_bank;
    int sound_num;

    int sound_id;
} Game;

void game_init(Game* game, System* system);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif