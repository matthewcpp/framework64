#pragma once

#include "framework64/util/arcball_camera.h"
#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#define MESH_COUNT 3

typedef struct {
    fw64Engine* engine;
    ArcballCamera arcball;

    fw64Font* consolas;
    fw64Texture* button_sprite;

    fw64Node node;
    int current_mesh;
    fw64Mesh* meshes[MESH_COUNT];

    int switch_model_text_width;
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
