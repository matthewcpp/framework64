#pragma once

#include <framework64/engine.h>
#include <framework64/transform.h>

#include "fw64_ui_navigation.h"

typedef enum {
    FW64_TRANSFORM_EDITOR_TARGET_POSX = 0,
    FW64_TRANSFORM_EDITOR_TARGET_POSY,
    FW64_TRANSFORM_EDITOR_TARGET_POSZ,
    FW64_TRANSFORM_EDITOR_TARGET_ROTX,
    FW64_TRANSFORM_EDITOR_TARGET_ROTY,
    FW64_TRANSFORM_EDITOR_TARGET_ROTZ,
    FW64_TRANSFORM_EDITOR_TARGET_SCALEX,
    FW64_TRANSFORM_EDITOR_TARGET_SCALEY,
    FW64_TRANSFORM_EDITOR_TARGET_SCALEZ,
    FW64_TRANSFORM_EDITOR_TARGET_COUNT
} fw64TransformEditorTarget;

typedef enum {
    FW64_TRANSFORM_EDITOR_MODE_SELECTING,
    FW64_TRANSFORM_EDITOR_MODE_EDITING
} fw64TransformEditorMode;

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64Node* target;
    fw64SpriteBatch* spritebatch;
    fw64UiNavigation ui_navigation;
    Vec3 euler_rotation;
    IVec2 ui_pos;
    Vec3 initial_position;
    Quat initial_rotation;
    Vec3 initial_scale;

    /** The index of the transform value to be edited. i.e. rot.x, scale.y, etc */
    int edit_target;

    /** Indicates whether the editor is selected the element to edit or actively editing it */
    fw64TransformEditorMode mode;

    /** Indicates if an edit was made since the last call to update.*/
    int did_edit;

    float position_adjust_amount;
    float rotation_adjust_amount;
    float scale_adjust_amount;
} fw64TransformEditor;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_transform_editor_init(fw64TransformEditor* editor, fw64Engine* engine, fw64Font* font, fw64Node* target, fw64Allocator* allocator);
void fw64_transform_editor_uninit(fw64TransformEditor* editor);
void fw64_transform_editor_update(fw64TransformEditor* editor);

/** Binds a target node this editor and captures it's initial values.
 *  Note: this will set the did_edit member variable to false;
 */
void fw64_transform_editor_set_target(fw64TransformEditor* editor, fw64Node* target);

/** Resets target's transform values to their inital state when it was bound to this editor.
 *  Note: this will set the did_edit member variable to true.
 */
void fw64_transform_editor_reset(fw64TransformEditor* editor);

#ifdef __cplusplus
}
#endif
