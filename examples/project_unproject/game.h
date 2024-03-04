#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "arcball_camera/arcball_camera.h"

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64ArcballCamera arcball_camera;
    fw64Scene scene;
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
