#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"
#include "fw64_headlight.h"
#include "fw64_ui_navigation.h"

#include "color_editor.h"
#include "light_editor.h"
#include "material_editor.h"

typedef enum {
    RENDER_PASS_SCENE,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} RenderPass;

typedef enum {
    GAME_MODE_SCENE,
    GAME_MODE_MENU
} GameMode;

typedef enum {
    SETTING_MATERIAL_COLOR,
    SETTING_LIGHT1,
    SETTING_LIGHT2,
    SETTING_LIGHT3,
    SETTING_INVALID
} SettingIndex;

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    SettingIndex setting_index;
    int setting_index_is_active;
    MaterialEditor material_editor;
    LightEditor light_editors[3];
    fw64UiNavigation ui_nav;
    fw64SpriteBatch* spritebatch;
    int is_active;
} Ui;

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
    GameMode mode;
    Ui ui;
    fw64ArcballCamera arcball;
    fw64Scene scene;
    fw64Headlights headlights;
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
