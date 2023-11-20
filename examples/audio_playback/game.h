#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Texture* buttons;
    fw64Font* font;
    fw64Node n64_logo;
    float music_playback_speed;
    float rotation;
    char tempo_text[32];
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
