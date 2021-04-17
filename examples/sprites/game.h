#ifndef GAME_H
#define GAME_H

#include "framework64/camera.h"
#include "framework64/engine.h"

#include "sprites.h"

typedef struct {
    fw64Engine* engine;
    Camera camera;
    KenSprite ken_sprite;
    N64LogoSprite n64logo;
    ElapsedTime elapsed_time;
    fw64Font basic_lazer;
} Game;

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif