#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "common/fps_camera.h"

typedef enum {
    GAME_MODE_PLAYING,
    GAME_MODE_PAUSED
} GameMode;

typedef enum {
    FOG_SETTING_ENABLED,
    FOG_SETTING_FOG_BEGIN,
    FOG_SETTING_FOG_END
} FogSetting;

typedef struct {
    fw64Engine* engine;
    fw64FpsCamera fps_camera;
    fw64Scene* scene;
    fw64Font* font;
    fw64Texture* overlay;
    GameMode mode;
    FogSetting fog_setting;
    float fog_begin;
    float fog_end;
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
