#include "color_editor.h"

#include "framework64/math.h"

void color_editor_init(ColorEditor* color_editor, fw64UiNavigation* ui_nav, fw64ColorRGBA8 initial_color, ColorEditorCallback callback, void* arg) {
    color_editor->ui_nav = ui_nav;
    color_editor->callback = callback;
    color_editor->callback_arg = arg;
    color_editor->current_color = initial_color;
    color_editor->component_index = 0;
}

#define EDIT_SPEED 225.0f

void color_editor_update(ColorEditor* color_editor, float time_delta) {
    float direction = 0.0f;

    if (fw64_ui_navigation_moved_up(color_editor->ui_nav)) {
        direction = 1.0f;
    } else if (fw64_ui_navigation_moved_down(color_editor->ui_nav)) {
        direction = -1.0f;
    } else if (fw64_ui_navigation_moved_right(color_editor->ui_nav)) {
        color_editor->component_index += 1;
    } else if (fw64_ui_navigation_moved_left(color_editor->ui_nav)) {
        color_editor->component_index -= 1;
    }

    if (color_editor->component_index < 0) {
        color_editor->component_index = 2;
    } else if (color_editor->component_index > 2) {
        color_editor->component_index = 0;
    }

    uint8_t* values = (uint8_t*)&color_editor->current_color;
    int delta = (int)(direction * EDIT_SPEED * time_delta);
    int new_value = (int)(values[color_editor->component_index]) + delta;

    values[color_editor->component_index] = (uint8_t)fw64_clampi(new_value, 0, 255);

    if (color_editor->callback) {
        color_editor->callback(color_editor->callback_arg);
    }
}