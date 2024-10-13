#include "material_editor.h"

static void material_editor_update_str(MaterialEditor* editor) {
    color_editor_update_str(&editor->color_editor, editor->display_str, "Material", editor->active);
}

static void material_editor_color_updated(void* arg) {
    MaterialEditor* editor = (MaterialEditor*)arg;

    fw64Mesh* mesh = editor->node->mesh_instance->mesh;
    uint32_t primitive_count = fw64_mesh_get_primitive_count(mesh);
    for (uint32_t i = 0; i < primitive_count; i++) {
        fw64Material* material = fw64_material_collection_get_material(fw64_mesh_get_material_collection(mesh), i);
        fw64_material_set_color(material, editor->color_editor.current_color);
    }

    material_editor_update_str(editor);
}

static fw64ColorRGBA8 material_editor_get_mesh_default_color(MaterialEditor* editor) {
    fw64Mesh* mesh = editor->node->mesh_instance->mesh;
    fw64Material* material = fw64_material_collection_get_material(fw64_mesh_get_material_collection(mesh), 0);
    return fw64_material_get_color(material);
}

void material_editor_init(MaterialEditor* editor, fw64UiNavigation* ui_nav, fw64Node* node, IVec2* pos, fw64Font* font){
    editor->node = node;
    editor->pos = *pos;
    editor->font = font;
    editor->active = 0;

    fw64ColorRGBA8 default_color = material_editor_get_mesh_default_color(editor);
    color_editor_init(&editor->color_editor, ui_nav, default_color, material_editor_color_updated, editor);
    material_editor_update_str(editor);
}

void material_editor_update_color(MaterialEditor* editor) {
    editor->color_editor.current_color = material_editor_get_mesh_default_color(editor);
    editor->color_editor.component_index = 0;
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
