#include "framework64/mesh.h"

#include "framework64/filesystem.h"

#include <malloc.h>
#include <string.h>

static void fixup_vertex_pointers(Mesh* mesh, uint32_t* vertex_pointer_data, int handle);
static void load_textures(Mesh* mesh, uint32_t* asset_index_data, int handle);

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

    uint32_t scratch_data_size = mesh->info._vertex_pointer_data_size > mesh->info.texture_count * sizeof(uint32_t) ? mesh->info._vertex_pointer_data_size : mesh->info.texture_count * sizeof(uint32_t);
    uint32_t* scratch_data = malloc(scratch_data_size);

    fixup_vertex_pointers(mesh, scratch_data, handle);
    load_textures(mesh, scratch_data, handle);

    free(scratch_data);

    filesystem_close(handle);

    return 1;
}

void mesh_unload(Mesh* mesh) {
    free(mesh->vertex_buffer);
    free(mesh->display_list);
    free(mesh->colors);
    free(mesh->primitives);

    if (mesh->textures) {
        for (uint32_t i = 0; i < mesh->info.texture_count; i++) {
            sprite_uninit(mesh->textures + i);
        }

        free(mesh->textures);
    }
}

static void fixup_vertex_pointers(Mesh* mesh, uint32_t* vertex_pointer_data, int handle) {
    filesystem_read(vertex_pointer_data, 1, mesh->info._vertex_pointer_data_size, handle);

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

static void load_textures(Mesh* mesh, uint32_t* asset_index_data, int handle) {
    if (mesh->info.texture_count > 0) {
        filesystem_read(asset_index_data, sizeof(uint32_t), mesh->info.texture_count, handle);
        mesh->textures = malloc(sizeof(ImageSprite) * mesh->info.texture_count);

        for (uint32_t i = 0; i < mesh->info.texture_count; i++) {
            sprite_load(asset_index_data[i], mesh->textures + i);
        }
    }
    else {
        mesh->textures = NULL;
    }
}
