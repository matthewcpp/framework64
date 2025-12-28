#include "fw64_transform_editor.h"

#include <framework64/matrix.h>
#include <framework64/controller_mapping/n64.h>

#include <stdio.h>

#define DEFAULT_EDITOR_POSITION_ADJUSTMENT_AMOUNT 1.0f
#define DEFAULT_EDITOR_ROTATION_ADJUSTMENT_AMOUNT 5.0f
#define DEFAULT_EDITOR_SCALE_ADJUSTMENT_AMOUNT 0.1f

static void update_spritebatch(fw64TransformEditor* editor);
static void adjust_edit_target(fw64TransformEditor* editor, int direction);
static void adjust_value(fw64TransformEditor* editor, float direction);

void fw64_transform_editor_init(fw64TransformEditor* editor, fw64Engine* engine, fw64Font* font, fw64Node* target, fw64Allocator* allocator) {
    editor->engine = engine;
    editor->font = font;
    editor->spritebatch = fw64_spritebatch_create(1, allocator);
    editor->mode = FW64_TRANSFORM_EDITOR_MODE_SELECTING;

    editor->position_adjust_amount = DEFAULT_EDITOR_POSITION_ADJUSTMENT_AMOUNT;
    editor->rotation_adjust_amount = DEFAULT_EDITOR_ROTATION_ADJUSTMENT_AMOUNT;
    editor->scale_adjust_amount = DEFAULT_EDITOR_SCALE_ADJUSTMENT_AMOUNT;

    fw64_ui_navigation_init(&editor->ui_navigation, engine->input, 0);

    ivec2_set_zero(&editor->ui_pos);

    fw64_transform_editor_set_target(editor, target);
}

void fw64_transform_editor_uninit(fw64TransformEditor* editor){
    fw64_spritebatch_delete(editor->spritebatch);
}

void fw64_transform_editor_set_target(fw64TransformEditor* editor, fw64Node* target) {
    editor->target = target;
    editor->mode = FW64_TRANSFORM_EDITOR_MODE_SELECTING;

    if (editor->target) {
        editor->euler_rotation = quat_to_euler(&target->transform.rotation);
        vec3_scale(&editor->euler_rotation, (180.0f / M_PI), &editor->euler_rotation);

        editor->initial_position = editor->target->transform.position;
        editor->initial_rotation = editor->target->transform.rotation;
        editor->initial_scale = editor->target->transform.scale;
    } else {
        vec3_set_zero(&editor->euler_rotation);
    }

    editor->edit_target = 0; 
    editor->did_edit = 0;
    update_spritebatch(editor);
}

void fw64_transform_editor_reset(fw64TransformEditor* editor) {
    if (!editor->target) {
        return;
    }

    editor->target->transform.position = editor->initial_position;
    editor->target->transform.rotation = editor->initial_rotation;
    editor->target->transform.scale = editor->initial_scale;

    fw64_node_update(editor->target);
    update_spritebatch(editor);
}

static void fw64_transform_editor_update_selecting(fw64TransformEditor* editor) {
    if (fw64_ui_navigation_accepted(&editor->ui_navigation)) {
        editor->mode = FW64_TRANSFORM_EDITOR_MODE_EDITING;
        update_spritebatch(editor);
        return;
    }

    if (fw64_ui_navigation_moved_right(&editor->ui_navigation)) {
        adjust_edit_target(editor, 1);
    } else if (fw64_ui_navigation_moved_down(&editor->ui_navigation)) {
        adjust_edit_target(editor, 3);
    } else if (fw64_ui_navigation_moved_up(&editor->ui_navigation)) {
        adjust_edit_target(editor, -3);
    } else if (fw64_ui_navigation_moved_left(&editor->ui_navigation)) {
        adjust_edit_target(editor, -1);
    }
}

static void fw64_transform_editor_update_editing(fw64TransformEditor* editor) {
    if (fw64_ui_navigation_accepted(&editor->ui_navigation) || fw64_ui_navigation_back(&editor->ui_navigation)) {
        editor->mode = FW64_TRANSFORM_EDITOR_MODE_SELECTING;
        update_spritebatch(editor);
        return;
    }

    if (fw64_ui_navigation_moved_up(&editor->ui_navigation) || fw64_ui_navigation_moved_right(&editor->ui_navigation)) {
        adjust_value(editor, 1.0f);
    } else if (fw64_ui_navigation_moved_down(&editor->ui_navigation) || fw64_ui_navigation_moved_left(&editor->ui_navigation)) {
        adjust_value(editor, -1.0f);
    }
}

void fw64_transform_editor_update(fw64TransformEditor* editor) {
    editor->did_edit = 0;
    if (!editor->target) {
        return;
    }

    fw64_ui_navigation_update(&editor->ui_navigation, editor->engine->time->time_delta);

    switch (editor->mode) {
        case FW64_TRANSFORM_EDITOR_MODE_SELECTING:
            fw64_transform_editor_update_selecting(editor);
            break;

        case FW64_TRANSFORM_EDITOR_MODE_EDITING:
            fw64_transform_editor_update_editing(editor);
            break;
    }
}

static void determine_indicators_for_target_type(fw64TransformEditor* editor, fw64TransformEditorTarget base_transform, char** indicators) {
    indicators[0] = " ";
    indicators[1] = " ";
    indicators[2] = " ";

    unsigned int edit_target = (unsigned int)editor->edit_target;

    if (edit_target < base_transform || edit_target > base_transform + 2) {
        return;
    }

    indicators[edit_target - base_transform] = editor->mode == FW64_TRANSFORM_EDITOR_MODE_EDITING ? ">" : "*";
}

void update_spritebatch(fw64TransformEditor* editor) {
    char buffer[32];

    fw64_spritebatch_begin(editor->spritebatch);

    IVec2 write_pos = editor->ui_pos;
    if (editor->target) {
        char* indicators[3];
        // write position
        determine_indicators_for_target_type(editor, FW64_TRANSFORM_EDITOR_TARGET_POSX, indicators);
        sprintf(buffer, "p: %s%.2f, %s%.2f, %s%.2f", indicators[0], editor->target->transform.position.x, indicators[1], editor->target->transform.position.y, indicators[2], editor->target->transform.position.z);
        fw64_spritebatch_draw_string(editor->spritebatch, editor->font, buffer, write_pos.x, write_pos.y);
        write_pos.y += fw64_font_line_height(editor->font);

        // write rotation
        determine_indicators_for_target_type(editor, FW64_TRANSFORM_EDITOR_TARGET_ROTX, indicators);
        sprintf(buffer, "r: %s%.2f, %s%.2f, %s%.2f", indicators[0], editor->euler_rotation.x, indicators[1], editor->euler_rotation.y, indicators[2], editor->euler_rotation.z);
        fw64_spritebatch_draw_string(editor->spritebatch, editor->font, buffer, write_pos.x, write_pos.y);
        write_pos.y += fw64_font_line_height(editor->font);

        // write scale
        determine_indicators_for_target_type(editor, FW64_TRANSFORM_EDITOR_TARGET_SCALEX, indicators);
        sprintf(buffer, "s: %s%.2f, %s%.2f, %s%.2f", indicators[0], editor->target->transform.scale.x, indicators[1], editor->target->transform.scale.y, indicators[2], editor->target->transform.scale.z);
        fw64_spritebatch_draw_string(editor->spritebatch, editor->font, buffer, write_pos.x, write_pos.y);
    }

    fw64_spritebatch_end(editor->spritebatch);
}

void adjust_value(fw64TransformEditor* editor, float direction) {
    switch (editor->edit_target) {
        case FW64_TRANSFORM_EDITOR_TARGET_POSX:
            editor->target->transform.position.x += editor->position_adjust_amount * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_POSY:
            editor->target->transform.position.y += editor->position_adjust_amount * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_POSZ:
            editor->target->transform.position.z += editor->position_adjust_amount * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_ROTX:
            editor->euler_rotation.x += editor->rotation_adjust_amount * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_ROTY:
            editor->euler_rotation.y += editor->rotation_adjust_amount * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_ROTZ:
            editor->euler_rotation.z += editor->rotation_adjust_amount * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_SCALEX:
            editor->target->transform.scale.x += editor->scale_adjust_amount * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_SCALEY:
            editor->target->transform.scale.y += editor->scale_adjust_amount * direction;
            break;
        case FW64_TRANSFORM_EDITOR_TARGET_SCALEZ:
            editor->target->transform.scale.z += editor->scale_adjust_amount * direction;
            break;
    }

    quat_from_euler(&editor->target->transform.rotation, editor->euler_rotation.x, editor->euler_rotation.y, editor->euler_rotation.z);
    fw64_node_update(editor->target);
    update_spritebatch(editor);
    editor->did_edit = 1;
}

void adjust_edit_target(fw64TransformEditor* editor, int direction) {
    editor->edit_target += direction;

    if (editor->edit_target < 0) {
        editor->edit_target = FW64_TRANSFORM_EDITOR_TARGET_COUNT + editor->edit_target;
    } else if (editor->edit_target >= FW64_TRANSFORM_EDITOR_TARGET_COUNT) {
        editor->edit_target -= FW64_TRANSFORM_EDITOR_TARGET_COUNT;
    }

    update_spritebatch(editor);
}
