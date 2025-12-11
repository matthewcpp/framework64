#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_headlight.h"
#include "fw64_debug_primitives.h"
#include "fw64_transform_editor.h"

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64Font* font;
    fw64Scene* scene;
    fw64DebugPrimitives* debug_primitives;
    int node_edit_index;
    fw64DebugPrimitiveHandle debug_handle;
    fw64TransformEditor editor;
} UI;

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* scene_renderpass;
    fw64RenderPass* debug_renderpass;
    fw64Scene* scene;
    fw64Camera camera;
    fw64Headlight headlight;
    fw64DebugPrimitives debug_primitives;

    UI ui;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_fixed_update(Game* game);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
