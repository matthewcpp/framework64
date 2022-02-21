#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;

    float rumble_frequency;
    float rumble_duration;

    fw64Mesh* quad_mesh;
    fw64Node node;

    float shake_speed;
    float shake_time;

    fw64Font* font;
    fw64Texture* buttons;
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
