#pragma once

#include "framework64/engine.h"
#include "framework64/input.h"

typedef struct {
    fw64Texture* buttons;
    fw64Font* font;
} Assets;

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    Assets* assets;
    int player_index;
} ControllerInfo;

typedef struct {
    fw64Engine* engine;
    ControllerInfo controller_infos[FW64_MAX_CONTROLLER_COUNT];
    Assets assets;
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
