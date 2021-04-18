#ifndef GAME_H
#define GAME_H

#include "framework64/engine.h"

typedef struct {
    fw64Engine* engine;
    Camera camera;
    fw64Texture* buttons;
    fw64Font* font;
    int sound_bank;
    int sound_num;
    int sound_id;

    int music_bank;
    int music_track;

    Entity n64_logo;
    float rotation;
} Game;

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif