#include "framework64/n64/mesh.h"

#include "framework64/filesystem.h"
#include "framework64/n64/image.h"
#include "framework64/n64/texture.h"

#include <malloc.h>
#include <string.h>

static void fixup_vertex_pointers(fw64Mesh* mesh, uint32_t* vertex_pointer_data, int handle);
static void fixup_material_texture_pointers(fw64Mesh* mesh);
static void load_textures(fw64Mesh* mesh, uint32_t* asset_index_data, int handle);
static uint32_t compute_scratch_data_buffer_size(fw64Mesh* mesh);

fw64Mesh* fw64_mesh_load(fw64AssetDatabase* database, uint32_t asset_index) {
    (void)database;

    int handle = fw64_filesystem_open(asset_index);
    if (handle < 0)
        return 0;

    fw64Mesh* mesh = malloc(sizeof(fw64Mesh));

    fw64_filesystem_read(&mesh->info, sizeof(fw64MeshInfo), 1, handle);

    mesh->vertex_buffer = memalign(8, sizeof(Vtx) * mesh->info.vertex_count );
    fw64_filesystem_read(mesh->vertex_buffer, sizeof(Vtx),  mesh->info.vertex_count, handle);

    mesh->display_list = memalign(8, sizeof(Gfx) * mesh->info.display_list_count);
    fw64_filesystem_read(mesh->display_list, sizeof(Gfx), mesh->info.display_list_count, handle);

    mesh->colors = memalign(8, sizeof(Lights1) * mesh->info.color_count);
    fw64_filesystem_read(mesh->colors, sizeof(Lights1), mesh->info.color_count, handle);

    mesh->primitives = malloc(sizeof(fw64Primitive) * mesh->info.primitive_count);
    fw64_filesystem_read(mesh->primitives, sizeof(fw64Primitive), mesh->info.primitive_count, handle);

    uint32_t scratch_data_size = compute_scratch_data_buffer_size(mesh);
    uint32_t* scratch_data = malloc(scratch_data_size);

    fixup_vertex_pointers(mesh, scratch_data, handle);
    load_textures(mesh, scratch_data, handle);
    fixup_material_texture_pointers(mesh);

    free(scratch_data);

    fw64_filesystem_close(handle);

    return mesh;
}

/** Calculates the amount of scratch data that will need to be allocated in order to contain all the variable length mesh data needed to load the file. */
static uint32_t compute_scratch_data_buffer_size(fw64Mesh* mesh) {
    uint32_t texture_scratch_data_size = mesh->info.texture_count * sizeof(uint32_t);
    
    return mesh->info._vertex_pointer_data_size > texture_scratch_data_size ? mesh->info._vertex_pointer_data_size : texture_scratch_data_size;
}

static void fixup_vertex_pointers(fw64Mesh* mesh, uint32_t* vertex_pointer_data, int handle) {
    fw64_filesystem_read(vertex_pointer_data, 1, mesh->info._vertex_pointer_data_size, handle);

    uint32_t* vertex_pointer_counts = vertex_pointer_data;
    uint32_t* vertex_pointer_offsets = vertex_pointer_data + mesh->info.primitive_count;

    uint32_t offset_index = 0;

    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        Vtx* vertex_buffer = mesh->vertex_buffer + primitive->vertices;
        Gfx* display_list = mesh->display_list + primitive->display_list;

        for (uint32_t j = 0; j < vertex_pointer_counts[i]; j++) {
            // updates the memory location of the vertex cache relative to the start of the vertex buffer for this primitive
            Gfx* vertex_ptr = display_list + vertex_pointer_offsets[offset_index++];
            vertex_ptr->words.w1 += (uint32_t)vertex_buffer;
        }
    }
}

static void load_textures(fw64Mesh* mesh, uint32_t* asset_index_data, int handle) {
    if (mesh->info.texture_count == 0) {
        mesh->textures = NULL;
        return;
    }

    fw64_filesystem_read(asset_index_data, sizeof(uint32_t), mesh->info.texture_count, handle);
    mesh->textures = malloc(sizeof(fw64Texture) * mesh->info.texture_count);

    for (uint32_t i = 0; i < mesh->info.texture_count; i++) {
        fw64Image* image = fw64_image_load(NULL, asset_index_data[i]);
        fw64_n64_texture_init_with_image(mesh->textures + i, image);
    }
}

/** When the mesh info is loaded from ROM, the value in the texture pointer represents an index into the mesh's texture array.
 * After the textures are loaded, we need to convert each material's texture index into an actual pointer to the correct texture object.
*/
static void fixup_material_texture_pointers(fw64Mesh* mesh) {
    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        fw64Material* material = &mesh->primitives[i].material;
        uint32_t texture_index = (uint32_t)material->texture;

        if (texture_index == FW64_MATERIAL_NO_TEXTURE) {
            material->texture = NULL;
        }
        else {
            material->texture = mesh->textures + texture_index;
        }
    }
}

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box) {
    *box = mesh->info.bounding_box;
}

void fw64_n64_mesh_init(fw64Mesh* mesh) {
    memset(mesh, 0, sizeof(fw64Mesh));
}

void fw64_mesh_delete(fw64Mesh* mesh) {
    if (mesh->vertex_buffer)
        free(mesh->vertex_buffer);

    if (mesh->display_list)
        free(mesh->display_list);
    
    if (mesh->colors)
        free(mesh->colors);

    if (mesh->primitives)
        free(mesh->primitives);

    if ( mesh->info.texture_count > 0) {
        for (uint32_t i = 0; i < mesh->info.texture_count; i++) {
            fw64_n64_texture_uninit(mesh->textures + i);
        }

        free(mesh->textures);
    }
}

fw64Primitive* fw64_mesh_get_primitive(fw64Mesh* mesh, int index) {
    if (index >= mesh->info.primitive_count)
        return NULL;
    else
        return mesh->primitives + index;
}

fw64Material* fw64_mesh_primitive_get_material(fw64Primitive* primitive) {
    return &primitive->material;
}

fw64Texture* fw64_material_get_texture(fw64Material* material) {
    return material->texture;
}

int fw64_mesh_get_texture_count(fw64Mesh* mesh) {
    return mesh->info.texture_count;
}

fw64Texture* fw64_mesh_get_texture(fw64Mesh* mesh, int index) {
    if (index >= mesh->info.texture_count)
        return NULL;
    else 
        return mesh->textures + index;
}