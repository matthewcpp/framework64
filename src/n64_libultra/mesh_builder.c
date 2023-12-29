#include "framework64/n64/mesh_builder.h"
#include "framework64/n64/mesh.h"
#include "framework64/n64/filesystem.h"

#include <stdint.h>
#include <string.h>

static void fw64_n64_mesh_builder_clear_data(fw64MeshBuilder* mesh_builder);

fw64MeshBuilder* fw64_mesh_builder_create(fw64AssetDatabase* assets, size_t primitive_count, size_t image_count, fw64Allocator* allocator) {
    fw64MeshBuilder* mesh_builder = allocator->malloc(allocator, sizeof(fw64MeshBuilder));
    memset(mesh_builder, 0, sizeof(fw64MeshBuilder));

    mesh_builder->allocator = allocator;

    fw64_mesh_builder_reset(mesh_builder, primitive_count, image_count);

    return mesh_builder;
}

void fw64_mesh_builder_reset(fw64MeshBuilder* mesh_builder, size_t primitive_count, size_t image_count) {
    fw64_n64_mesh_builder_clear_data(mesh_builder);

    fw64Allocator* allocator = mesh_builder->allocator;
    mesh_builder->material_bundle = allocator->malloc(allocator, sizeof(fw64MaterialBundle));
    fw64_n64_material_bundle_init(mesh_builder->material_bundle, image_count, image_count, primitive_count, allocator);

    for (uint32_t i = 0; i < primitive_count; i++) {
        fw64_n64_material_init(mesh_builder->material_bundle->materials + i);
    }

    mesh_builder->primitive_infos = allocator->malloc(allocator, sizeof(fw64N64PrimitiveInfo) * primitive_count);
    mesh_builder->primitive_info_count = primitive_count;

    mesh_builder->next_image_index = 0;
    mesh_builder->active_primitive_index = SIZE_MAX;
}

void fw64_n64_mesh_builder_clear_data(fw64MeshBuilder* mesh_builder) {
    fw64Allocator* allocator = mesh_builder->allocator;
    if (mesh_builder->material_bundle) {
        fw64_material_bundle_delete(mesh_builder->material_bundle, allocator);
    }

    if (mesh_builder->primitive_infos) {
        for (size_t i = 0; i < mesh_builder->primitive_info_count; i++) {
            fw64N64PrimitiveInfo* info = mesh_builder->primitive_infos + i;

            if (info->vertices) {
                allocator->free(allocator, info->vertices);
            }
            
            if (info->display_list) {
                allocator->free(allocator, info->display_list);
            }
        }

        allocator->free(allocator, mesh_builder->primitive_infos);
    }
}

void fw64_mesh_builder_delete(fw64MeshBuilder* mesh_builder) {
    fw64_n64_mesh_builder_clear_data(mesh_builder);

    mesh_builder->allocator->free(mesh_builder->allocator, mesh_builder);
}

fw64Texture* fw64_mesh_builder_load_image(fw64MeshBuilder* mesh_builder, fw64AssetId asset_id) {
    fw64N64FilesystemDataSource data_source;
    if (!fw64_n64_filesystem_open_datasource(&data_source, asset_id))
        return NULL;

    fw64Image* image = mesh_builder->material_bundle->images + mesh_builder->next_image_index;
    fw64_n64_image_read_data(image, &data_source.interface, mesh_builder->allocator);

    fw64Texture* texture = mesh_builder->material_bundle->textures + mesh_builder->next_image_index;
    fw64_n64_texture_init_with_image(texture, image);

    fw64_n64_filesystem_close_datasource(&data_source);
    mesh_builder->next_image_index += 1;

    return texture;
}

fw64Material* fw64_mesh_builder_get_material(fw64MeshBuilder* mesh_builder, size_t index) {
    return mesh_builder->material_bundle->materials + index;
}

int fw64_mesh_builder_allocate_primitive_quad_data(fw64MeshBuilder* mesh_builder, size_t index, fw64MeshBuilderVertexAttributes vertex_attributes, size_t count) {
    fw64Allocator* allocator = mesh_builder->allocator;

    // TODO: check if already allocated and reset
    fw64N64PrimitiveInfo* info = mesh_builder->primitive_infos + index;
    info->vertex_count = count * 4;
    info->vertices = allocator->memalign(allocator, 8, info->vertex_count * sizeof(Vtx));

    // Vertex cache can hold 32 verts -> 8 quads of 4 vertices each
    size_t total_chunk_count = count % 8 == 0 ? count / 8 : (count / 8) + 1;

    // each chuck needs 9 display list entries -> gSPVertex + 8 gSP2Triangles plus a final gSPEndDisplayList
    info->display_list_count = total_chunk_count * 9 + 1;
    info->display_list = allocator->memalign(allocator, 8, info->display_list_count * sizeof(Gfx));

    Gfx* display_list = info->display_list;
    Vtx* vertices = info->vertices;

    size_t quads_remaining = count;
    while (quads_remaining > 0) {
        size_t chunk_quad_count = quads_remaining > 8 ? 8 : quads_remaining;
        size_t chunk_vertices_count = chunk_quad_count * 4;

        gSPVertex(display_list++, vertices, chunk_vertices_count, 0);

        for (size_t i = 0; i < chunk_quad_count; i++) {
            size_t vertex_offset = i * 4;
            gSP2Triangles(display_list++, 
                vertex_offset + 0, vertex_offset + 1, vertex_offset + 2, 0,
                vertex_offset + 0, vertex_offset + 2, vertex_offset + 3, 0);
        }

        vertices += chunk_vertices_count;
        quads_remaining -= chunk_quad_count;
    }

    gSPEndDisplayList(display_list++);

    return 1;
}

int fw64_mesh_builder_allocate_primitive_data(fw64MeshBuilder* mesh_builder, size_t index, fw64PrimitiveMode mode, fw64MeshBuilderVertexAttributes vertex_attributes, size_t vertex_count, size_t element_count) {
    if (vertex_count > 32) {
        return 0;
    }

    fw64Allocator* allocator = mesh_builder->allocator;

    // TODO: check if already allocated and reset
    fw64N64PrimitiveInfo* info = mesh_builder->primitive_infos + index;
    info->vertex_count = vertex_count;
    info->vertices = allocator->memalign(allocator, 8, info->vertex_count * sizeof(Vtx));

    info->display_list_count = element_count + 2;
    info->display_list = allocator->memalign(allocator, 8, info->display_list_count * sizeof(Gfx));

    gSPVertex(info->display_list, info->vertices, vertex_count, 0);
    gSPEndDisplayList(info->display_list + info->display_list_count - 1);

    return 1;
}

void fw64_mesh_builder_set_triangle_vertex_indices(fw64MeshBuilder* mesh_builder, size_t index, uint16_t a, uint16_t b, uint16_t c) {
    fw64N64PrimitiveInfo* active_primitive_info = mesh_builder->primitive_infos + mesh_builder-> active_primitive_index;
    gSP1Triangle(active_primitive_info->display_list + index + 1, a, b, c, 0);
}

void fw64_mesh_builder_set_line_vertex_indices(fw64MeshBuilder* mesh_builder, size_t index, uint16_t a, uint16_t b) {
    fw64N64PrimitiveInfo* active_primitive_info = mesh_builder->primitive_infos + mesh_builder-> active_primitive_index;
    gSPLine3D(active_primitive_info->display_list + index + 1, a, b, 0);
}


int fw64_mesh_builder_set_active_primitive(fw64MeshBuilder* mesh_builder, size_t index) {
    if (index < mesh_builder->primitive_info_count) {
        mesh_builder->active_primitive_index = index;
        return 1;
    }
    
    return 0;
}

void fw64_mesh_builder_set_vertex_position_f(fw64MeshBuilder* mesh_builder, size_t index, float x, float y, float z) {
    fw64N64PrimitiveInfo* active_primitive_info = mesh_builder->primitive_infos + mesh_builder-> active_primitive_index;
    Vtx* vertex = active_primitive_info->vertices + index;
    
    vertex->v.ob[0] = (short)x;
    vertex->v.ob[1] = (short)y;
    vertex->v.ob[2] = (short)z;
    vertex->v.flag = 0;
}

void fw64_mesh_builder_set_vertex_normal_f(fw64MeshBuilder* mesh_builder, size_t index, float x, float y, float z) {
    fw64N64PrimitiveInfo* active_primitive_info = mesh_builder->primitive_infos + mesh_builder-> active_primitive_index;
    Vtx* vertex = active_primitive_info->vertices + index;

    x = x * 128.0f;
    x = x < 127.0f ? x : 127.0f;

    y = y * 128.0f;
    y = x < 127.0f ? y : 127.0f;

    z = z * 128.0f;
    z = z < 127.0f ? z : 127.0f;

    vertex->n.n[0] = (signed char)x;
    vertex->n.n[1] = (signed char)y;
    vertex->n.n[2] = (signed char)z;
}

void fw64_mesh_builder_set_vertex_color_rgba8(fw64MeshBuilder* mesh_builder, size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    fw64N64PrimitiveInfo* active_primitive_info = mesh_builder->primitive_infos + mesh_builder-> active_primitive_index;
    Vtx* vertex = active_primitive_info->vertices + index;

    vertex->v.cn[0] = r;
    vertex->v.cn[1] = g;
    vertex->v.cn[2] = b;
    vertex->v.cn[3] = a;
}

void fw64_mesh_builder_set_vertex_texcoords_f(fw64MeshBuilder* mesh_builder, size_t index, float s, float t) {
    fw64N64PrimitiveInfo* active_primitive_info = mesh_builder->primitive_infos + mesh_builder-> active_primitive_index;
    Vtx* vertex = active_primitive_info->vertices + index;
    fw64Material* material = mesh_builder->material_bundle->materials + mesh_builder->active_primitive_index;

    s *= (float)fw64_texture_slice_width(material->texture) * 2.0f;
    t *= (float)fw64_texture_slice_height(material->texture) * 2.0f;

    // Note that the texture coordinates (s,t) are encoded in S10.5 format.
    vertex->v.tc[0] =  (short)(s * 32.0f);
    vertex->v.tc[1] =  (short)(t * 32.0f);
}

void fw64_mesh_builder_set_bounding(fw64MeshBuilder* mesh_builder, Box* bounding) {
    mesh_builder->bounding = *bounding;
}

fw64Mesh* fw64_mesh_builder_commit(fw64MeshBuilder* mesh_builder) {
    fw64Allocator* allocator = mesh_builder->allocator;

    fw64Mesh* mesh = allocator->malloc(allocator, sizeof(fw64Mesh));
    fw64_n64_mesh_init(mesh);
    
    mesh->info.primitive_count = mesh_builder->primitive_info_count;
    mesh->info.vertex_count = mesh_builder->primitive_infos->vertex_count;
    mesh->info.display_list_count = mesh_builder->primitive_infos->display_list_count;
    mesh->info._material_bundle_count = 1;
    mesh->info.bounding_box = mesh_builder->bounding;

    mesh->primitives = allocator->malloc(allocator, mesh->info.primitive_count * sizeof(fw64Primitive));
    mesh->material_bundle = mesh_builder->material_bundle;
    mesh->vertex_buffer = mesh_builder->primitive_infos->vertices;
    mesh->display_list = mesh_builder->primitive_infos->display_list;

    // TODO: need to support multiple primtives
    mesh->primitives->vertices = mesh_builder->primitive_infos->vertices;
    mesh->primitives->display_list = mesh_builder->primitive_infos->display_list;
    mesh->primitives->joint_index = FW64_JOINT_INDEX_NONE;
    mesh->primitives->material = mesh->material_bundle->materials;

    // clear out constructed data
    mesh_builder->material_bundle = NULL;

    for (size_t i = 0; i < mesh_builder->primitive_info_count; i++) {
        fw64N64PrimitiveInfo* info = mesh_builder->primitive_infos + i;

        info->vertices = NULL;
        info->display_list = NULL;
    }

    return mesh;
}