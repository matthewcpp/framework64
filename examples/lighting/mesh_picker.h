#pragma once

#include "framework64/font.h"
#include "framework64/scene.h"
#include "framework64/sprite_batch.h"

#include "fw64_ui_navigation.h"

typedef void (*MeshPickerCallback)(void* arg);

typedef struct {
    fw64Scene* scene;
    fw64UiNavigation* ui_nav;
    fw64Font* font;
    int mesh_index;
    IVec2 pos;
    char display_str[32];
    int active;
    MeshPickerCallback callback;
    void* callback_arg;
} MeshPicker;

void mesh_picker_init(MeshPicker* picker, fw64Scene* scene, fw64UiNavigation* ui_nav, fw64Font* font, IVec2* pos, MeshPickerCallback callback, void* arg);
void mesh_picker_update(MeshPicker* picker);
void mesh_picker_draw(MeshPicker* picker, fw64SpriteBatch* spritebatch);
void mesh_picker_activate(MeshPicker* picker);
void mesh_picker_deactivate(MeshPicker* picker);
