#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

#include "arcball_camera/arcball_camera.h"

typedef struct {
    fw64Engine* engine;
    fw64ArcballCamera arcball;
    fw64Font* font;
    fw64Node node;
    int selected_material;
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
