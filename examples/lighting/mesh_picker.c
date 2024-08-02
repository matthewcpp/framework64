#include "mesh_picker.h"

#include "assets/assets.h"

typedef struct {
    const char* name;
    fw64AssetId asset_id;
} MeshInfo;

#define MESH_COUNT 2

static const MeshInfo mesh_infos[MESH_COUNT] = {
    {"suzanne", FW64_ASSET_mesh_suzanne},
    {"penguin", FW64_ASSET_mesh_penguin}
};

void mesh_picker_init(MeshPicker* picker, fw64Scene* scene, fw64UiNavigation* ui_nav) {
    picker->scene = scene;
    picker->ui_nav = ui_nav;
    picker->mesh_index = 0;
}

void mesh_picker_update(MeshPicker* picker) {
    (void)picker;
}
