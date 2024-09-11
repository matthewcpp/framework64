#include "mesh_picker.h"

#include "assets/assets.h"

#include <stdio.h>

typedef struct {
    const char* name;
    fw64AssetId asset_id;
} MeshInfo;

#define MESH_COUNT 2

static const MeshInfo mesh_infos[MESH_COUNT] = {
    {"suzanne", FW64_ASSET_mesh_suzanne},
    {"penguin", FW64_ASSET_mesh_penguin}
};

static void mesh_picker_update_display_str(MeshPicker* picker) {
    const char* label = picker->active ? ">" : " ";
    sprintf(picker->display_str, "Mesh: %s%s", label, mesh_infos[picker->mesh_index].name);
}

void mesh_picker_init(MeshPicker* picker, fw64Scene* scene, fw64UiNavigation* ui_nav, fw64Font* font, IVec2* pos, MeshPickerCallback callback, void* arg) {
    picker->scene = scene;
    picker->ui_nav = ui_nav;
    picker->mesh_index = 0;
    picker->pos = *pos;
    picker->font = font;
    picker->active = 0;
    picker->callback = callback;
    picker->callback_arg = arg;

    mesh_picker_update_display_str(picker);
}

void mesh_picker_update(MeshPicker* picker) {
    int previous_mesh_index = picker->mesh_index;

    if (fw64_ui_navigation_moved_up(picker->ui_nav)) {
        picker->mesh_index -= 1;
    } else if (fw64_ui_navigation_moved_down(picker->ui_nav)) {
        picker->mesh_index += 1;
    }

    if (picker->mesh_index == previous_mesh_index) {
        return;
    }

    if (picker->mesh_index < 0) {
        picker->mesh_index = MESH_COUNT - 1;
    } else if (picker->mesh_index >= MESH_COUNT) {
        picker->mesh_index = 0;
    }

    fw64Mesh* mesh = fw64_scene_get_mesh(picker->scene, picker->mesh_index);
    fw64MeshInstance* mesh_instance = fw64_scene_get_mesh_instance(picker->scene, 0);
    fw64_mesh_instance_set_mesh(mesh_instance, mesh);
    mesh_picker_update_display_str(picker);

    if (picker->callback) {
        picker->callback(picker->callback_arg);
    }
}

void mesh_picker_draw(MeshPicker* picker, fw64SpriteBatch* spritebatch) {
    fw64_spritebatch_draw_string(spritebatch, picker->font, picker->display_str, picker->pos.x, picker->pos.y);
}

void mesh_picker_activate(MeshPicker* picker) {
    picker->active = 1;
    mesh_picker_update_display_str(picker);
}

void mesh_picker_deactivate(MeshPicker* picker) {
    picker->active = 0;
    mesh_picker_update_display_str(picker);
}
