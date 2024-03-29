#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Texture* buttons;
    fw64Font* font;
    int sound_bank;
    int sound_num;
    int sound_id;

    int music_bank;
    int music_track;

    fw64Node n64_logo;
    float rotation;
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
