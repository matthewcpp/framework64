#include "material_editor.h"

#include <stdio.h>

static void material_editor_update_str(MaterialEditor* editor) {
    ColorEditor* color_editor = &editor->color_editor;
    const char* r_label = " ", *g_label = " ", *b_label = " ";

    if (editor->active) {
        switch(editor->color_editor.component_index) {
            case 0:
                r_label = ">";
                break;
            case 1:
                g_label = ">";
                break;
            case 2:
                b_label = ">";
                break;
        }
    }

    sprintf(editor->display_str, "Material: %s%d, %s%d, %s%d", 
        r_label, color_editor->current_color.r, 
        g_label, color_editor->current_color.g, 
        b_label, color_editor->current_color.b);
}

static void material_editor_color_updated(void* arg) {
    MaterialEditor* editor = (MaterialEditor*)arg;

    int primitive_count = fw64_mesh_get_primitive_count(editor->mesh);
    for (int i = 0; i < primitive_count; i++) {
        fw64Material* material = fw64_mesh_get_material_for_primitive(editor->mesh, i);
        fw64_material_set_color(material, editor->color_editor.current_color);
    }

    material_editor_update_str(editor);
}

void material_editor_init(MaterialEditor* editor, fw64UiNavigation* ui_nav, fw64Mesh* mesh, IVec2* pos, fw64Font* font){
    editor->mesh = mesh;
    editor->pos = *pos;
    editor->font = font;
    editor->active = 0;

    fw64Material* material = fw64_mesh_get_material_for_primitive(mesh, 0);
    editor->default_color = fw64_material_get_color(material);
    color_editor_init(&editor->color_editor, ui_nav, editor->default_color, material_editor_color_updated, editor);
    material_editor_update_str(editor);
}

void material_editor_draw(MaterialEditor* editor, fw64SpriteBatch* spritebatch) {
    fw64_spritebatch_draw_string(spritebatch, editor->font, editor->display_str, editor->pos.x, editor->pos.y);
}

void material_editor_update(MaterialEditor* editor, float time_delta) {
    color_editor_update(&editor->color_editor, time_delta);
}

void material_editor_activate(MaterialEditor* editor) {
    editor->active = 1;
    material_editor_update_str(editor);
}

void material_editor_deactivate(MaterialEditor* editor) {
    editor->active = 0;
    material_editor_update_str(editor);
}
