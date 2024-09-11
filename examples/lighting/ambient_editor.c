#include "ambient_editor.h"

static void ambient_editor_update_str(AmbientEditor* editor) {
    color_editor_update_str(&editor->color_editor, editor->display_str, "Ambient", editor->active);
}

static void ambient_editor_color_updated(void* arg) {
    AmbientEditor* editor = (AmbientEditor*)arg;

    fw64_renderpass_set_lighting_ambient_color(editor->renderpass, editor->color_editor.current_color);
    ambient_editor_update_str(editor);
}

void ambient_editor_init(AmbientEditor* editor, fw64UiNavigation* ui_nav, fw64RenderPass* renderpass, IVec2* pos, fw64Font* font){
    editor->renderpass = renderpass;
    editor->pos = *pos;
    editor->font = font;
    editor->active = 0;

    fw64ColorRGBA8 ambient_light_color = {55, 55, 55, 255};
    color_editor_init(&editor->color_editor, ui_nav, ambient_light_color, ambient_editor_color_updated, editor);
    ambient_editor_update_str(editor);
}

void ambient_editor_draw(AmbientEditor* editor, fw64SpriteBatch* spritebatch) {
    fw64_spritebatch_draw_string(spritebatch, editor->font, editor->display_str, editor->pos.x, editor->pos.y);
}

void ambient_editor_update(AmbientEditor* editor, float time_delta) {
    color_editor_update(&editor->color_editor, time_delta);
}

void ambient_editor_activate(AmbientEditor* editor) {
    editor->active = 1;
    ambient_editor_update_str(editor);
}

void ambient_editor_deactivate(AmbientEditor* editor) {
    editor->active = 0;
    ambient_editor_update_str(editor);
}
