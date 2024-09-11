#pragma once

#include "framework64/color.h"

#include "fw64_ui_navigation.h"

typedef void (*ColorEditorCallback)(void* arg);

typedef struct {
    fw64UiNavigation* ui_nav;
    fw64ColorRGBA8 current_color;
    ColorEditorCallback callback;
    void* callback_arg;
    int component_index;
} ColorEditor;

void color_editor_init(ColorEditor* color_editor, fw64UiNavigation* ui_nav, fw64ColorRGBA8 initial_color, ColorEditorCallback callback, void* arg);
void color_editor_update(ColorEditor* color_editor, float time_delta);

void color_editor_update_str(ColorEditor* color_editor, char* buffer, const char* label, int active);
