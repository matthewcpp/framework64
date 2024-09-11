#pragma once

#include "framework64/font.h"
#include "framework64/render_pass.h"
#include "framework64/sprite_batch.h"

#include "color_editor.h"

typedef struct {
    fw64RenderPass* renderpass;
    fw64Font* font;
    IVec2 pos;
    ColorEditor color_editor;
    char display_str[32];
    int active;
} AmbientEditor;

void ambient_editor_init(AmbientEditor* editor, fw64UiNavigation* ui_nav, fw64RenderPass* renderpass, IVec2* pos, fw64Font* font);
void ambient_editor_update(AmbientEditor* editor, float time_delta);
void ambient_editor_draw(AmbientEditor* editor, fw64SpriteBatch* spritebatch);
void ambient_editor_activate(AmbientEditor* editor);
void ambient_editor_deactivate(AmbientEditor* editor);
