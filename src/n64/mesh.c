#include "framework64/mesh.h"

#include "framework64/filesystem.h"

#include <malloc.h>
#include <string.h>

void fixup_vertex_pointers(Mesh* mesh, uint32_t* vertex_pointer_data);

int mesh_load(int asset_index, Mesh* mesh) {
    int handle = filesystem_open(asset_index);
    if (handle < 0)
        return 0;

    filesystem_read(&mesh->info, sizeof(MeshInfo), 1, handle);

    mesh->vertex_buffer = memalign(8, sizeof(Vtx) * mesh->info.vertex_count );
    filesystem_read(mesh->vertex_buffer, sizeof(Vtx),  mesh->info.vertex_count, handle);

    mesh->display_list = memalign(8, sizeof(Gfx) * mesh->info.display_list_count);
    filesystem_read(mesh->display_list, sizeof(Gfx), mesh->info.display_list_count, handle);

    mesh->colors = memalign(8, sizeof(Lights1) * mesh->info.color_count);
    filesystem_read(mesh->colors, sizeof(Lights1), mesh->info.color_count, handle);

    mesh->primitives = malloc(sizeof(Primitive) * mesh->info.primitive_count);
    filesystem_read(mesh->primitives, sizeof(Primitive), mesh->info.primitive_count, handle);

    uint32_t* vertex_pointer_data = malloc(mesh->info._vertex_pointer_data_size);
    filesystem_read(vertex_pointer_data, 1, mesh->info._vertex_pointer_data_size, handle);

    fixup_vertex_pointers(mesh, vertex_pointer_data);
    free(vertex_pointer_data);

    filesystem_close(handle);

    return 1;
}

void mesh_unload(Mesh* mesh) {
    free(mesh->vertex_buffer);
    free(mesh->display_list);
    free(mesh->colors);
    free(mesh->primitives);
}

void fixup_vertex_pointers(Mesh* mesh, uint32_t* vertex_pointer_data) {
    uint32_t* vertex_pointer_counts = vertex_pointer_data;
    uint32_t* vertex_pointer_offsets = vertex_pointer_data + mesh->info.primitive_count;

    uint32_t offset_index = 0;

    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        Primitive* primitive = mesh->primitives + i;

        Vtx* vertex_buffer = mesh->vertex_buffer + primitive->vertices;
        Gfx* display_list = mesh->display_list + primitive->display_list;

        for (uint32_t j = 0; j < vertex_pointer_counts[i]; j++) {
            // updates the memory location of the vertex cache relative to the start of the vertex buffer for this primitive
            Gfx* vertex_ptr = display_list + vertex_pointer_offsets[offset_index++];
            vertex_ptr->words.w1 += (uint32_t)vertex_buffer;
        }
    }
}

void mesh_compute_bounding_box(Mesh* mesh, Box* box) {
    box_invalidate(box);

    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        box_encapsulate_box(box, &mesh->primitives[i].bounding_box);
    }
}