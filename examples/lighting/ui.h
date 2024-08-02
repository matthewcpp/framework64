#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"

#include "ambient_editor.h"
#include "color_editor.h"
#include "light_editor.h"
#include "material_editor.h"
#include "mesh_picker.h"

typedef enum {
    SETTING_MESH_PICKER,
    SETTING_MATERIAL_COLOR,
    SETTING_LIGHT1,
    SETTING_LIGHT2,
    SETTING_LIGHT3,
    SETTING_AMBIENT,
    SETTING_INVALID
} SettingIndex;

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    SettingIndex setting_index;
    int setting_index_is_active;
    MeshPicker mesh_picker;
    MaterialEditor material_editor;
    LightEditor light_editors[3];
    AmbientEditor ambient_editor;
    fw64UiNavigation ui_nav;
    fw64SpriteBatch* spritebatch;
    fw64ArcballCamera* arcball_camera;
    int is_active;
} Ui;

void ui_init(Ui* ui, fw64Engine* engine, fw64Scene* scene, fw64RenderPass* scene_renderpass, fw64Headlights* headlights, fw64ArcballCamera* arcball_camera);
void ui_update(Ui* ui);
void ui_draw(Ui* ui, fw64RenderPass* renderpass);
void ui_activate(Ui* ui);
