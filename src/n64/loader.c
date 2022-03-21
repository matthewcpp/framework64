#include "framework64/n64/loader.h"

#include "framework64/filesystem.h"
#include "framework64/n64/image.h"
#include "framework64/n64/texture.h"

#include <malloc.h>
#include <string.h>


static void fixup_mesh_vertex_pointers(fw64Mesh* mesh, int handle, fw64Allocator* allocator);
static void fixup_mesh_primitive_material_pointers(fw64N64Loader* loader, fw64Mesh* mesh);

void fw64_n64_loader_load_mesh_resources(fw64N64Loader* loader, int handle, fw64Allocator* allocator) {
    fw64MeshResources* resources = allocator->malloc(allocator, sizeof(fw64MeshResources));
    fw64_filesystem_read(resources, FW64_MESH_RESOURCES_HEADER_SIZE, 1, handle);
    resources->flags = 0;

    if (resources->image_count > 0) {
        // read the asset ID's and then load the images
        // TODO: get rid of this allocation when adding a small read optimization in the filesystem
        uint32_t* asset_index_data = allocator->malloc(allocator, resources->image_count * sizeof(uint32_t));
        fw64_filesystem_read(asset_index_data, sizeof(uint32_t), resources->image_count, handle);
        resources->images = allocator->malloc(allocator, sizeof(fw64Image) * resources->image_count);

        for (uint32_t i = 0; i < resources->image_count; i++) {
            fw64_n64_image_init_from_rom(resources->images + i, asset_index_data[i], FW64_IMAGE_FLAG_NONE, allocator);
        }

        allocator->free(allocator, asset_index_data);
    }
    else {
        resources->images = NULL;
    }

    if (resources->texture_count > 0) {
        resources->textures = allocator->malloc(allocator, resources->texture_count * sizeof(fw64Texture));
        fw64_filesystem_read(resources->textures , sizeof(fw64Texture), resources->texture_count, handle);
    }
    else {
        resources->textures = NULL;
    }

    if (resources->material_count > 0) { // this should always be the case
        resources->materials = allocator->malloc(allocator, sizeof(fw64Material) * resources->material_count);
        fw64_filesystem_read(resources->materials, sizeof(fw64Material), resources->material_count, handle);
    }
    else {
        resources->materials = NULL;
    }

    // fixup image pointers for textures
    for (uint32_t i = 0; i < resources->texture_count; i++) {
        fw64Texture* texture = resources->textures + i;

        uint32_t image_index = (uint32_t)texture->image;
        texture->image = resources->images + image_index;
    }

    // fixup texture pointers for materials
    for (uint32_t i = 0; i < resources->material_count; i++) {
        fw64Material* material = resources->materials + i;

        uint32_t material_texture_index = (uint32_t)material->texture;
        if (material_texture_index == FW64_MATERIAL_NO_TEXTURE)
            material->texture = NULL;
        else
            material->texture = resources->textures + material_texture_index;
    }

    loader->resources = resources;
}

void fw64_n64_loader_init(fw64N64Loader* loader) {
    memset(loader, 0, sizeof(fw64N64Loader));
}

void fw64_n64_loader_uninit(fw64N64Loader* loader) {
    (void)loader;
}

void fw64_n64_loader_load_mesh(fw64N64Loader* loader, fw64Mesh* mesh, int handle, fw64Allocator* allocator) {
    fw64_filesystem_read(&mesh->info, sizeof(fw64MeshInfo), 1, handle);

    mesh->vertex_buffer = allocator->memalign(allocator, 8, sizeof(Vtx) * mesh->info.vertex_count );
    fw64_filesystem_read(mesh->vertex_buffer, sizeof(Vtx),  mesh->info.vertex_count, handle);

    mesh->display_list = allocator->memalign(allocator, 8, sizeof(Gfx) * mesh->info.display_list_count);
    fw64_filesystem_read(mesh->display_list, sizeof(Gfx), mesh->info.display_list_count, handle);

    mesh->primitives = allocator->malloc(allocator, sizeof(fw64Primitive) * mesh->info.primitive_count);
    fw64_filesystem_read(mesh->primitives, sizeof(fw64Primitive), mesh->info.primitive_count, handle);
    
    fixup_mesh_vertex_pointers(mesh, handle, allocator);
    fixup_mesh_primitive_material_pointers(loader, mesh);
}


static void fixup_mesh_vertex_pointers(fw64Mesh* mesh, int handle, fw64Allocator* allocator) {
    //TODO: Get rid of this allocation when filesystem has a small read optimization
    uint32_t* vertex_pointer_data = allocator->malloc(allocator, mesh->info._vertex_pointer_data_size);
    fw64_filesystem_read(vertex_pointer_data, 1, mesh->info._vertex_pointer_data_size, handle);

    uint32_t* vertex_pointer_counts = vertex_pointer_data;
    uint32_t* vertex_pointer_offsets = vertex_pointer_data + mesh->info.primitive_count;

    uint32_t offset_index = 0;

    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        uint32_t vertices_index = (uint32_t)primitive->vertices;
        uint32_t display_list_index = (uint32_t)primitive->display_list;

        Vtx* vertex_buffer = mesh->vertex_buffer + vertices_index;
        Gfx* display_list = mesh->display_list + display_list_index;

        for (uint32_t j = 0; j < vertex_pointer_counts[i]; j++) {
            // updates the memory location of the vertex cache relative to the start of the vertex buffer for this primitive
            Gfx* vertex_ptr = display_list + vertex_pointer_offsets[offset_index++];
            vertex_ptr->words.w1 += (uint32_t)vertex_buffer;
        }

        primitive->vertices = mesh->vertex_buffer + vertices_index;
        primitive->display_list = mesh->display_list + display_list_index;
    }

    allocator->free(allocator, vertex_pointer_data);
}

/** When the mesh info is loaded from ROM, the value in material and texture pointers represents an index into top level mesh arrays.
 * After the textures are loaded, we need to convert each index into an actual pointer to the correct object type.
*/
static void fixup_mesh_primitive_material_pointers(fw64N64Loader* loader, fw64Mesh* mesh) {
    // fixup material pointers for primitives
    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        uint32_t primitive_material_index = (uint32_t)primitive->material;
        // TODO: Investigate this further.  This indicates that this primitive contains lines
        if (primitive_material_index == FW64_PRIMITIVE_NO_MATERIAL)
            primitive->material == NULL;
        else
            primitive->material = loader->resources->materials + primitive_material_index;
    }
}
