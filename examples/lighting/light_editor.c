#include "light_editor.h"

#include <stdio.h>

static const char* light_mode_names[LIGHT_MODE_COUNT] = {
    "Off",
    "Top",
    "Bottom",
    "Left,",
    "Right",
    "Front",
    "Back",
    "Head"
};

static void light_editor_update_display_str(LightEditor* editor) {
    if (editor->light_mode == LIGHT_MODE_OFF) {
        sprintf(editor->display_str, "Light %d: %s", editor->light_index, light_mode_names[editor->light_mode]);
    } else {
        sprintf(editor->display_str, "Light %d: %s", editor->light_index, light_mode_names[editor->light_mode]);
    }
}

static void light_editor_adjust_light_params(LightEditor* editor) {
    int enabled = 1;
    Vec3 light_dir;

    switch (editor->light_mode) {
        case LIGHT_MODE_OFF:
        case LIGHT_MODE_COUNT:
            enabled = 0;
            break;
        case LIGHT_MODE_TOP:
            vec3_set(&light_dir, 0.0f, -1.0f, 0.0f);
            break;
        case LIGHT_MODE_BOTTOM:
            vec3_set(&light_dir, 0.0f, 1.0f, 0.0f);
            break;
        case LIGHT_MODE_LEFT:
            vec3_set(&light_dir, 1.0f, 0.0f, 0.0f);
            break;
        case LIGHT_MODE_RIGHT:
            vec3_set(&light_dir, -1.0f, 0.0f, 0.0f);
            break;
        case LIGHT_MODE_FRONT:
            vec3_set(&light_dir, 0.0f, 0.0f, -1.0f);
            break;
        case LIGHT_MODE_BACK:
            vec3_set(&light_dir, 0.0f, 0.0f, 1.0f);
            break;
        case LIGHT_MODE_HEADLIGHT:
            fw64_headlights_create(editor->headlights, editor->renderpass, editor->light_index, editor->camera_transform, &editor->headlight_handle);
            break;
    }

    fw64_renderpass_set_light_enabled(editor->renderpass, editor->light_index, enabled);

    if (enabled && editor->light_mode != LIGHT_MODE_HEADLIGHT) {
        fw64_headlights_delete(editor->headlights, editor->headlight_handle);
        fw64_renderpass_set_light_direction(editor->renderpass, editor->light_index, &light_dir);
    }

    light_editor_update_display_str(editor);
}

void light_editor_init(LightEditor* editor, fw64UiNavigation* ui_nav, fw64Font* font, IVec2* pos, fw64RenderPass* renderpass, int light_index, LightMode inital_mode, fw64Headlights* headlights, fw64Transform* camera_transform) {
    editor->ui_nav = ui_nav;
    editor->renderpass = renderpass;
    editor->light_index = light_index;
    editor->active = 0;
    editor->light_mode = inital_mode;
    editor->headlight_handle = fw64_sparse_set_invalid_handle();
    editor->pos = *pos;
    editor->font = font;
    editor->headlights = headlights;
    editor->camera_transform = camera_transform;

    light_editor_adjust_light_params(editor);
}

void light_editor_update(LightEditor* editor, float time_delta) {
    int previous_mode = editor->light_mode;
    if (fw64_ui_navigation_moved_up(editor->ui_nav)) {
        editor->light_mode += 1;
    } else if (fw64_ui_navigation_moved_down(editor->ui_nav)) {
        editor->light_mode -= 1;
    }

    if (editor->light_mode >= LIGHT_MODE_COUNT) {
        editor->light_mode = 0;
    } else if (editor->light_mode < 0) {
        editor->light_mode = LIGHT_MODE_COUNT - 1;
    }

    if (editor->light_mode != previous_mode) {
        light_editor_adjust_light_params(editor);
    }
    
    (void)time_delta;
}

void light_editor_draw(LightEditor* editor, fw64SpriteBatch* spritebatch) {
    fw64_spritebatch_draw_string(spritebatch, editor->font, editor->display_str, editor->pos.x, editor->pos.y);
}

void light_editor_activate(LightEditor* editor) {
    editor->active = 1;
}

void light_editor_deactivate(LightEditor* editor) {
    editor->active = 0;
}
