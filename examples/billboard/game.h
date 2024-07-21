#pragma once

#include "framework64/engine.h"
#include "framework64/render_pass.h"
#include "framework64/scene.h"
#include "framework64/util/quad.h"

#include "fw64_billboard_node.h"
#include "fw64_fps_camera.h"

typedef enum {
    RENDERPASS_DEPTH_ENABLED,
    RENDERPASS_DEPTH_DISABLED,
    RENDERPASS_COUNT
} RenderPass;

typedef struct {
    float update_time_remaining;
    fw64Mesh* mesh;
} Flame;

typedef struct {
    fw64Engine* engine;
    fw64FpsCamera fps;
    Flame flame;
    fw64Scene* scene;
    fw64RenderPass* renderpass[RENDERPASS_COUNT];
    fw64BillboardNodes billboard_nodes;
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
