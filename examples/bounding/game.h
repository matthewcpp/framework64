#ifndef GAME_H
#define GAME_H

#include "framework64/engine.h"

typedef enum {
    CUBE_TOP_LEFT,
    CUBE_TOP_RIGHT,
    CUBE_BOTTOM_LEFT,
    CUBE_BOTTOM_RIGHT,
    CUBE_COUNT,
    CUBE_NONE = CUBE_COUNT
} CubeId;

typedef struct {
    fw64Engine* engine;
    Camera camera;
    Entity penguin;
    Entity penguin_box;
    Entity cubes[CUBE_COUNT];
    CubeId intersection;
    Font* font;
} Game;

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif