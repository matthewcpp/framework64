#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "arcball_camera/arcball_camera.h"

typedef enum {
    FLAG_NONE = 0,
    FLAG_HIT_POS_ACTIVE = 1
} Flags;

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64ArcballCamera arcball_camera;
    fw64Scene scene;
    uint32_t flags;
    fw64Texture* crosshair;
    IVec2 crosshair_pos, min_pos, max_pos, hit_pos;
    fw64RaycastHit raycast;
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
