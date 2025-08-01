#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"
#include "fw64_headlight.h"

typedef struct {
    fw64ColorRGBA8 hairColor, skinColor;
    fw64Texture* textures[4];
} AlternatePalette;

typedef struct {
    fw64Engine* engine;
    fw64Scene scene;
    fw64Camera camera;
    fw64ArcballCamera arcball;
    fw64Headlight headlight;
    fw64RenderPass* renderpass;
    fw64MaterialCollection alternate_palette;
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
