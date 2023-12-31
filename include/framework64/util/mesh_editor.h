#pragma once

/** \file mesh_editor.h */

#include "framework64/mesh.h"

#include <stdint.h>

typedef struct {
    fw64Mesh* mesh;
    fw64Primitive* primitive;
    uint32_t flags;
} fw64MeshEditor;

#ifdef __cplusplus
extern "C" {
#endif


/** Initializes a new mesh editor with the supplied mesh. */
void fw64_mesh_editor_init(fw64MeshEditor* mesh_editor, fw64Mesh* mesh);

/**
 * Sets the active primitive for data editing.
 * Returns zero if the supplied index is out of range, otherwise returns non-zero.
 */
int fw64_mesh_editor_set_active_primitive(fw64MeshEditor* mesh_editor, size_t index);

/** 
 * Commits changes made to the mesh. 
 * This performs any platform specific tasks associated with editing mesh data.
 * When editing data across multiple primitives, this method should be called once for each primitive.
 */
void fw64_mesh_editor_commit(fw64MeshEditor* mesh_editor);

void fw64_mesh_editor_set_vertex_positionf(fw64MeshEditor* mesh_editor, size_t index, float x, float y, float z);
void fw64_mesh_editor_set_vertex_normal_f(fw64MeshEditor* mesh_editor, size_t index, float x, float y, float z);
void fw64_mesh_editor_set_vertex_color_rgba8(fw64MeshEditor* mesh_editor, size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void fw64_mesh_editor_set_vertex_texcoords_f(fw64MeshEditor* mesh_editor, size_t index, float s, float t);

#ifdef __cplusplus
}
#endif
