#include "fw64_transform_editor.h"

#include <framework64/matrix.h>
#include <framework64/controller_mapping/n64.h>

#include <stdio.h>

static void update_spritebatch(fw64TransformEditor* editor);
static void adjust_edit_target(fw64TransformEditor* editor, int direction);
static void adjust_value(fw64TransformEditor* editor, float direction);

void fw64_transform_editor_init(fw64TransformEditor* editor, fw64Engine* engine, fw64Font* font, fw64Transform* target, fw64Allocator* allocator) {
    editor->engine = engine;
    editor->font = font;
    editor->spritebatch = fw64_spritebatch_create(1, allocator);
    editor->edit_target = 0; 

    editor->target = target;
    fw64_ui_navigation_init(&editor->ui_navigation, engine->input, 0);

    editor->euler_rotation = quat_to_euler(&target->rotation);
    vec3_scale(&editor->euler_rotation, (180.0f / M_PI), &editor->euler_rotation);

    ivec2_set_zero(&editor->ui_pos);
    update_spritebatch(editor);
}

void fw64_transform_editor_uninit(fw64TransformEditor* editor){
    fw64_spritebatch_delete(editor->spritebatch);
}

void fw64_transform_editor_update(fw64TransformEditor* editor) {
    fw64_ui_navigation_update(&editor->ui_navigation, editor->engine->time->time_delta);

    if (fw64_ui_navigation_moved_up(&editor->ui_navigation)) {
        adjust_edit_target(editor, -1);
    } else if (fw64_ui_navigation_moved_down(&editor->ui_navigation)) {
        adjust_edit_target(editor, 1);
    } 

    else if (fw64_ui_navigation_moved_right(&editor->ui_navigation)) {
        adjust_value(editor, 1.0f);
    } else if (fw64_ui_navigation_moved_left(&editor->ui_navigation)) {
        adjust_value(editor, -1.0f);
    }
}

void update_spritebatch(fw64TransformEditor* editor) {
    char buffer[32];

    fw64_spritebatch_begin(editor->spritebatch);

    sprintf(buffer, "rot: %.2f, %.2f, %.2f", editor->euler_rotation.x, editor->euler_rotation.y, editor->euler_rotation.z);
    fw64_spritebatch_draw_string(editor->spritebatch, editor->font, buffer, editor->ui_pos.x, editor->ui_pos.y);
    
    fw64_spritebatch_end(editor->spritebatch);
}

void adjust_value(fw64TransformEditor* editor, float direction) {
    switch (editor->edit_target) {
        case FW64_TRANSFORM_EDITOR_TARGET_ROTX:
            editor->euler_rotation.x += 5.0f * direction;
            break;
        
        case FW64_TRANSFORM_EDITOR_TARGET_ROTY:
            editor->euler_rotation.y += 5.0f * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_ROTZ:
            editor->euler_rotation.z += 5.0f * direction;
            break;
    }

    quat_from_euler(&editor->target->rotation, editor->euler_rotation.x, editor->euler_rotation.y, editor->euler_rotation.z);
    fw64_transform_update_matrix(editor->target);
    update_spritebatch(editor);
}

void adjust_edit_target(fw64TransformEditor* editor, int direction) {
    editor->edit_target += direction;

    if (editor->edit_target < 0) {
        editor->edit_target = FW64_TRANSFORM_EDITOR_TARGET_COUNT - 1;
    } else if (editor->edit_target >= FW64_TRANSFORM_EDITOR_TARGET_COUNT) {
        editor->edit_target = 0;
    }
}
