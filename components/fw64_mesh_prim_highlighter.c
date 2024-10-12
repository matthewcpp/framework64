#include "fw64_mesh_prim_highlighter.h"

static void fw64_mesh_prim_highlighter_highlight_active_prim(fw64MeshPrimHighlighter* highlighter);
static void fw64_mesh_prim_highlighter_revert_active_prim(fw64MeshPrimHighlighter* highlighter);

void fw64_mesh_prim_highlighter_init(fw64MeshPrimHighlighter* highlighter, fw64MeshInstance* mesh_instance, fw64Allocator* allocator){
    highlighter->mesh_instance = mesh_instance;
    highlighter->prim_index = 0;
    fw64_color_rgba8_set(&highlighter->highlight_color, 255, 255, 0, 255);

    highlighter->existing_collection = fw64_mesh_instance_get_material_collection(mesh_instance);
    fw64_material_collection_init_clone(&highlighter->collection, highlighter->existing_collection, allocator);
    fw64_mesh_instance_set_material_collection(mesh_instance, &highlighter->collection);

    fw64_mesh_prim_highlighter_highlight_active_prim(highlighter);
}

void fw64_mesh_prim_highlighter_uninit(fw64MeshPrimHighlighter* highlighter) {
    fw64_mesh_instance_set_material_collection(highlighter->mesh_instance, highlighter->existing_collection);
    fw64_material_collection_uninit(&highlighter->collection, highlighter->allocator);
}

void fw64_mesh_prim_highlighter_revert_active_prim(fw64MeshPrimHighlighter* highlighter) {
    fw64MaterialCollection* material_collection = fw64_mesh_instance_get_material_collection(highlighter->mesh_instance);
    fw64Material* material = fw64_material_collection_get_material(material_collection, highlighter->prim_index);

    fw64_material_set_color(material, highlighter->existing_color);
}

void fw64_mesh_prim_highlighter_highlight_active_prim(fw64MeshPrimHighlighter* highlighter) {
    fw64MaterialCollection* material_collection = fw64_mesh_instance_get_material_collection(highlighter->mesh_instance);
    fw64Material* material = fw64_material_collection_get_material(material_collection, highlighter->prim_index);
    
    highlighter->existing_color = fw64_material_get_color(material);
    fw64_material_set_color(material, highlighter->highlight_color);
}

void fw64_mesh_prim_highlighter_next(fw64MeshPrimHighlighter* highlighter) {
    fw64_mesh_prim_highlighter_revert_active_prim(highlighter);

    highlighter->prim_index += 1;
    if (highlighter->prim_index >= (int)fw64_mesh_get_primitive_count(highlighter->mesh_instance->mesh)) {
        highlighter->prim_index = 0;
    }

    fw64_mesh_prim_highlighter_highlight_active_prim(highlighter);
}

void fw64_mesh_prim_highlighter_previous(fw64MeshPrimHighlighter* highlighter) {
    fw64_mesh_prim_highlighter_revert_active_prim(highlighter);

    highlighter->prim_index -= 1;
    if (highlighter->prim_index < 0) {
        highlighter->prim_index = (int)fw64_mesh_get_primitive_count(highlighter->mesh_instance->mesh) - 1;
    }

    fw64_mesh_prim_highlighter_highlight_active_prim(highlighter);
}
