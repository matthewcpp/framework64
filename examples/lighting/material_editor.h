#pragma once

#include "framework64/font.h"
#include "framework64/node.h"
#include "framework64/sprite_batch.h"

#include "color_editor.h"

typedef struct {
    fw64Node* node;
    fw64Font* font;
    IVec2 pos;
    ColorEditor color_editor;
    char display_str[32];
    int active;
} MaterialEditor;

void material_editor_init(MaterialEditor* editor, fw64UiNavigation* ui_nav, fw64Node* node, IVec2* pos, fw64Font* font);
void material_editor_update(MaterialEditor* editor, float time_delta);
void material_editor_draw(MaterialEditor* editor, fw64SpriteBatch* spritebatch);
void material_editor_activate(MaterialEditor* editor);
void material_editor_deactivate(MaterialEditor* editor);

void material_editor_update_color(MaterialEditor* editor);

