#pragma once

#include "framework64/scene.h"

#include "fw64_ui_navigation.h"

typedef struct {
    fw64Scene* scene;
    fw64UiNavigation* ui_nav;
    int mesh_index;
} MeshPicker;

void mesh_picker_init(MeshPicker* picker, fw64Scene* scene, fw64UiNavigation* ui_nav);
void mesh_picker_update(MeshPicker* picker);
