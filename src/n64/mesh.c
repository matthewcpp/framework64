#include "framework64/n64/mesh.h"

#include "framework64/n64/loader.h"

#include "framework64/filesystem.h"

#include <malloc.h>
#include <string.h>

fw64Mesh* fw64_mesh_load(fw64AssetDatabase* assets, uint32_t index) {
    (void)assets;
    fw64N64Loader loader;
    fw64_n64_loader_init(&loader);

    int handle = fw64_filesystem_open(index);
    if (handle < 0)
        return NULL;

    fw64_n64_loader_load_mesh_resources(&loader, handle);
    fw64Mesh* mesh = malloc(sizeof(fw64Mesh));
    fw64_n64_loader_load_mesh(&loader, mesh, handle);
    mesh->resources = loader.resources;

    fw64_filesystem_close(handle);

    fw64_n64_loader_uninit(&loader);

    return mesh;
}

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box) {
    *box = mesh->info.bounding_box;
}

void fw64_n64_mesh_init(fw64Mesh* mesh) {
    memset(mesh, 0, sizeof(fw64Mesh));
}

void fw64_n64_mesh_uninit(fw64Mesh* mesh) {
    if (mesh->vertex_buffer)
        free(mesh->vertex_buffer);

    if (mesh->display_list)
        free(mesh->display_list);

    if (mesh->primitives)
        free(mesh->primitives);

    if (mesh->resources) {
        fw64_n64_mesh_resources_delete(mesh->resources);
    }
}

void fw64_mesh_delete(fw64Mesh* mesh) {
    fw64_n64_mesh_uninit(mesh);
    free(mesh);
}

int fw64_mesh_get_primitive_count(fw64Mesh* mesh) {
    return mesh->info.primitive_count;
}

fw64Material* fw64_mesh_get_material_for_primitive(fw64Mesh* mesh, int index) {
    fw64Primitive* primitive = mesh->primitives + index;

    return primitive->material;
}

void fw64_n64_mesh_resources_delete(fw64MeshResources* resources) {
    if (resources->material_count > 0) {
        free(resources->materials);
    }

    if (resources->texture_count > 0) {
        free(resources->textures);
    }

    int delete_images = !(resources->flags & FW64_MESH_FLAGS_IMAGES_ARE_SHARED);

    if (delete_images && resources->image_count > 0) {
        for (uint32_t i = 0; i < resources->image_count; i++) {
            fw64Image* image = resources->images + i;
            free(image->data);
        }
        
        free(resources->images);
    }

    free(resources);
}