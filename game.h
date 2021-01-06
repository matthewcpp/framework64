#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include "input.h"
#include "renderer.h"

typedef struct {
    Camera* camera;
    Renderer* renderer;
    Input* input;
} Game;

Game* game_create(Renderer* renderer, Input* input);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif