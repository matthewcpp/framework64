#ifndef GAME_H
#define GAME_H

#include "framework64/camera.h"
#include "framework64/system.h"

#include "sprites.h"

typedef struct {
    System* system;
    Camera camera;
    KenSprite ken_sprite;
    N64LogoSprite n64logo;
    ElapsedTime elapsed_time;
    Font basic_lazer;
} Game;

void game_init(Game* game, System* system);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif