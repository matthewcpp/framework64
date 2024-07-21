#include "debug_boxes.h"


void fw64_debug_boxes_init(fw64DebugBoxes* debug_boxes, int box_count, fw64Scene* scene, fw64Mesh* mesh, fw64Allocator* allocator) {
    debug_boxes->scene = scene;
    debug_boxes->allocator = allocator;
    debug_boxes->mesh = mesh;

    fw64_static_vector_init(&debug_boxes->entries, sizeof(fw64DebugBoxEntry), box_count, allocator);
}

void fw64_debug_boxes_uninit(fw64DebugBoxes* debug_boxes) {
    fw64_static_vector_uninit(&debug_boxes->entries, debug_boxes->allocator);
}

fw64Node* fw64_debug_boxes_add(fw64DebugBoxes* debug_boxes, fw64MeshInstance* source) {
    fw64Node* dest_node = fw64_scene_create_node(debug_boxes->scene);

    if (!dest_node) {
        return NULL;
    }

    fw64MeshInstance* mesh_instance = fw64_scene_create_mesh_instance(debug_boxes->scene, dest_node, debug_boxes->mesh);

    if (!mesh_instance) {
        return NULL;
    }

    fw64DebugBoxEntry* entry = (fw64DebugBoxEntry*)fw64_static_vector_alloc_back(&debug_boxes->entries);
    entry->src = source;
    entry->dest = mesh_instance;

    return dest_node;
}

void fw64_debug_boxes_update(fw64DebugBoxes* debug_boxes) {
    for (size_t i = 0; i < fw64_static_vector_size(&debug_boxes->entries); i++) {
        fw64DebugBoxEntry* entry = (fw64DebugBoxEntry*)fw64_static_vector_get_item(&debug_boxes->entries, i);
        box_center(&entry->src->render_bounds, &entry->dest->node->transform.position);
        box_extents(&entry->src->render_bounds, &entry->dest->node->transform.scale);
        fw64_node_update(entry->dest->node);
    }
}
