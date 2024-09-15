#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_fps_camera.h"
#include "fw64_ui_navigation.h"

typedef enum {
    GAME_MODE_PLAYING,
    GAME_MODE_PAUSED
} GameMode;

typedef enum {
    FOG_SETTING_ENABLED,
    FOG_SETTING_FOG_BEGIN,
    FOG_SETTING_FOG_END,
    FOG_SETTING_COUNT
} FogSetting;

typedef struct {
    int enabled;
    float begin;
    float end;
    fw64ColorRGBA8 color;
    fw64Renderer* renderer;
    fw64RenderPass* renderpass;
} FogSettings;

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    fw64Font* font;
    fw64Texture* overlay;

    fw64UiNavigation nav;
    FogSettings* fog_settings;
    int active_setting;
    int active;
} Ui;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64FpsCamera fps_camera;
    fw64Scene* scene;
    fw64RenderPass* renderpass;
    FogSettings fog_settings;
    GameMode mode;
    Ui ui;
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
