#pragma once

#include "framework64/render_pass.h"
#include "framework64/sprite_batch.h"

#include "fw64_headlight.h"
#include "fw64_ui_navigation.h"

#include "color_editor.h"

typedef enum {
    LIGHT_MODE_OFF,
    LIGHT_MODE_TOP,
    LIGHT_MODE_BOTTOM,
    LIGHT_MODE_LEFT,
    LIGHT_MODE_RIGHT,
    LIGHT_MODE_FRONT,
    LIGHT_MODE_BACK,
    LIGHT_MODE_HEADLIGHT,
    LIGHT_MODE_COUNT
} LightMode;

typedef enum {
    LIGHT_EDIT_TARGET_MODE,
    LIGHT_EDIT_TARGET_COLOR
} LightEditTarget;

typedef struct {
    int active;
    fw64UiNavigation* ui_nav;
    LightEditTarget edit_target;
    fw64RenderPass* renderpass;
    int light_index;
    int light_mode;
    fw64Transform* camera_transform;
    fw64Headlights* headlights;
    fw64HeadlightHandle headlight_handle;
    IVec2 pos;
    fw64Font* font;
    ColorEditor color_editor;
    char display_str[32];
} LightEditor;

void light_editor_init(LightEditor* editor, fw64UiNavigation* ui_nav, fw64Font* font, IVec2* pos, fw64RenderPass* renderpass, int light_index, LightMode inital_mode, fw64Headlights* headlights, fw64Transform* camera_transform);
void light_editor_update(LightEditor* editor, float time_delta);
void light_editor_draw(LightEditor* editor, fw64SpriteBatch* spritebatch);
void light_editor_activate(LightEditor* editor);
void light_editor_deactivate(LightEditor* editor);
