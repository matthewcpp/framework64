#pragma once

#include <framework64/engine.h>
#include <framework64/transform.h>

#include "fw64_ui_navigation.h"

typedef enum {
    FW64_TRANSFORM_EDITOR_TARGET_ROTX = 0,
    FW64_TRANSFORM_EDITOR_TARGET_ROTY,
    FW64_TRANSFORM_EDITOR_TARGET_ROTZ,
    FW64_TRANSFORM_EDITOR_TARGET_COUNT
} fw64TransformEditorTarget;

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64Transform* target;
    fw64SpriteBatch* spritebatch;
    fw64UiNavigation ui_navigation;
    Vec3 euler_rotation;
    IVec2 ui_pos;
    int edit_target;
} fw64TransformEditor;

void fw64_transform_editor_init(fw64TransformEditor* editor, fw64Engine* engine, fw64Font* font, fw64Transform* target, fw64Allocator* allocator);
void fw64_transform_editor_uninit(fw64TransformEditor* editor);
void fw64_transform_editor_update(fw64TransformEditor* editor);
