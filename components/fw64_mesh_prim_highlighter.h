#pragma once

#include "framework64/mesh_instance.h"

typedef struct {
    fw64MeshInstance* mesh_instance;
    fw64ColorRGBA8 highlight_color;
    fw64ColorRGBA8 existing_color;
    int prim_index;
    fw64Allocator* allocator;
    fw64MaterialCollection collection;
    fw64MaterialCollection* existing_collection;
} fw64MeshPrimHighlighter;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_mesh_prim_highlighter_init(fw64MeshPrimHighlighter* highlighter, fw64MeshInstance* mesh_instance, fw64Allocator* allocator);
void fw64_mesh_prim_highlighter_uninit(fw64MeshPrimHighlighter* highlighter);

void fw64_mesh_prim_highlighter_next(fw64MeshPrimHighlighter* highlighter);
void fw64_mesh_prim_highlighter_previous(fw64MeshPrimHighlighter* highlighter);

#ifdef __cplusplus
}
#endif
