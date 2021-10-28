#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

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
    fw64Camera camera;
    fw64Node penguin;
    fw64Collider penguin_collider;
    fw64Node penguin_box;
    fw64Node cubes[CUBE_COUNT];
    fw64Collider cube_colliders[CUBE_COUNT];
    CubeId intersection;
    fw64Font* font;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
