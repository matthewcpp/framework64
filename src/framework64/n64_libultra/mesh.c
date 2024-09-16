#include "framework64/n64_libultra/mesh.h"

#include "framework64/n64_libultra/asset_database.h"
#include "framework64/filesystem.h"

#include <malloc.h>
#include <string.h>

static void fixup_mesh_vertex_pointers(fw64Mesh* mesh, fw64DataSource* data_source, fw64Allocator* allocator);
static void fixup_mesh_primitive_material_pointers(fw64MaterialBundle* material_bundle, fw64Mesh* mesh);
static fw64Mesh* load_mesh_data(fw64DataSource* data_source, fw64MeshInfo* mesh_info, fw64MaterialBundle* material_bundle, fw64Allocator* allocator);

fw64Mesh* fw64_mesh_load_from_datasource(fw64AssetDatabase* assets, fw64DataSource* data_source, fw64Allocator* allocator) {
    fw64MeshInfo mesh_info;
    fw64_data_source_read(data_source, &mesh_info, sizeof(fw64MeshInfo), 1);

    if (mesh_info._material_bundle_count != 1) {
        return NULL;
    }

    fw64MaterialBundle* material_bundle = fw64_material_bundle_load_from_datasource(data_source, assets, allocator);

    if (!material_bundle) {
        return NULL;
    }

    fw64Mesh* mesh = load_mesh_data(data_source, &mesh_info, material_bundle, allocator);

    if (!mesh) {
        fw64_material_bundle_delete(material_bundle, allocator);
    }

    return mesh;
}

fw64Mesh* fw64_mesh_load_from_datasource_with_bundle(fw64AssetDatabase* assets, fw64DataSource* data_source, fw64MaterialBundle* material_bundle, fw64Allocator* allocator) {
    (void)assets;
    fw64MeshInfo mesh_info;
    fw64_data_source_read(data_source, &mesh_info, sizeof(fw64MeshInfo), 1);

    return load_mesh_data(data_source, &mesh_info, material_bundle, allocator);
}

fw64Mesh* load_mesh_data(fw64DataSource* data_source, fw64MeshInfo* mesh_info, fw64MaterialBundle* material_bundle, fw64Allocator* allocator) {
    fw64Mesh* mesh = fw64_allocator_malloc(allocator, sizeof(fw64Mesh));
    memcpy(&mesh->info, mesh_info, sizeof(fw64MeshInfo));
    
    mesh->vertex_buffer = fw64_allocator_memalign(allocator, 8, sizeof(Vtx) * mesh->info.vertex_count );
    fw64_data_source_read(data_source, mesh->vertex_buffer, sizeof(Vtx),  mesh->info.vertex_count);

    mesh->display_list = fw64_allocator_memalign(allocator, 8, sizeof(Gfx) * mesh->info.display_list_count);
    fw64_data_source_read(data_source, mesh->display_list, sizeof(Gfx), mesh->info.display_list_count);

    mesh->primitives = fw64_allocator_malloc(allocator, sizeof(fw64Primitive) * mesh->info.primitive_count);
    fw64_data_source_read(data_source, mesh->primitives, sizeof(fw64Primitive), mesh->info.primitive_count);
    
    fixup_mesh_vertex_pointers(mesh, data_source, allocator);
    fixup_mesh_primitive_material_pointers(material_bundle, mesh);

    mesh->material_bundle = material_bundle;

    return mesh;
}

static void fixup_mesh_vertex_pointers(fw64Mesh* mesh, fw64DataSource* data_source, fw64Allocator* allocator) {
    //TODO: Get rid of this allocation when filesystem has a small read optimization
    uint32_t* vertex_pointer_data = fw64_allocator_malloc(allocator, mesh->info._vertex_pointer_data_size);
    fw64_data_source_read(data_source, vertex_pointer_data, 1, mesh->info._vertex_pointer_data_size);

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

    fw64_allocator_free(allocator, vertex_pointer_data);
}

/** When the mesh info is loaded from ROM, the value in material and texture pointers represents an index into top level mesh arrays.
 * After the textures are loaded, we need to convert each index into an actual pointer to the correct object type.
*/
static void fixup_mesh_primitive_material_pointers(fw64MaterialBundle* material_bundle, fw64Mesh* mesh) {
    // fixup material pointers for primitives
    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        uint32_t primitive_material_index = (uint32_t)primitive->material;
        // TODO: Investigate this further.  This indicates that this primitive contains lines
        if (primitive_material_index == FW64_PRIMITIVE_NO_MATERIAL) {
            primitive->material = NULL;
        }
        else {
            primitive->material = material_bundle->materials + primitive_material_index;
        }
    }
}

void fw64_mesh_delete(fw64Mesh* mesh, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    (void)assets;

    fw64_n64_mesh_uninit(mesh, allocator);
    fw64_allocator_free(allocator, mesh);
}

Box fw64_mesh_get_bounding_box(fw64Mesh* mesh) {
    return mesh->info.bounding_box;
}

void fw64_n64_mesh_init(fw64Mesh* mesh) {
    memset(mesh, 0, sizeof(fw64Mesh));
}

void fw64_n64_mesh_uninit(fw64Mesh* mesh, fw64Allocator* allocator) {
    if (mesh->vertex_buffer) {
        fw64_allocator_free(allocator, mesh->vertex_buffer);
    }

    if (mesh->display_list) {
        fw64_allocator_free(allocator, mesh->display_list);
    }

    if (mesh->primitives) {
        fw64_allocator_free(allocator, mesh->primitives);
    }

    // this signifies that the material bundle was loaded with the mesh and therefore it is our responsibility to delete it here
    if (mesh->info._material_bundle_count > 0) {
        fw64_material_bundle_delete(mesh->material_bundle, allocator);
    }
}

uint32_t fw64_mesh_get_primitive_count(fw64Mesh* mesh) {
    return mesh->info.primitive_count;
}

fw64Material* fw64_mesh_get_material_for_primitive(fw64Mesh* mesh, uint32_t index) {
    fw64Primitive* primitive = mesh->primitives + index;

    return primitive->material;
}

fw64PrimitiveMode fw64_mesh_primitive_get_mode(fw64Mesh* mesh, uint32_t index) {
    fw64Primitive* primitive = mesh->primitives + index;
    Gfx* command = primitive->display_list + 1;

    if (command->line.cmd == G_LINE3D) {
        return FW64_PRIMITIVE_MODE_LINES;
    } else {
        return FW64_PRIMITIVE_MODE_TRIANGLES;
    }
}
