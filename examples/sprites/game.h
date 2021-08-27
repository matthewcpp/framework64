#ifndef GAME_H
#define GAME_H

#include "framework64/camera.h"
#include "framework64/engine.h"

#include "sprites.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    KenSprite ken_sprite;
    N64LogoSprite n64logo;
    ElapsedTime elapsed_time;
} Game;

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#endif