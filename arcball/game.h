#ifndef GAME_H
#define GAME_H

#include "arcball.h"
#include "ultra/camera.h"
#include "ultra/input.h"
#include "ultra/renderer.h"
#include "ultra/texture.h"

typedef struct {
    ArcballCamera* arcball;
    Camera* camera;
    Renderer* renderer;
    Input* input;
    Texture* texture;
} Game;

Game* game_create(Renderer* renderer, Input* input);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif