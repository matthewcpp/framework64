#include "framework64/util/quad.h"
#include "framework64/n64/image.h"
#include "framework64/n64/texture.h"
#include "framework64/n64/mesh.h"

#include <nusys.h>
#include <string.h>

static void vertex_set_tc(Vtx* vertex, float s, float t, fw64Texture* texture) {
    s *= (float)fw64_texture_slice_width(texture) * 2.0f;
    t *= (float)fw64_texture_slice_height(texture) * 2.0f;

    // Note that the texture coordinates (s,t) are encoded in S10.5 format.
    short ss = (short)s;
    short st = (short)t;
    vertex->n.tc[0] =  ss << 5;
    vertex->n.tc[1] = st << 5;
}

static void vertex_set_p_tc(Vtx* vertex, short x, short y, short z, float s, float t, fw64Texture* texture) {
    vertex->n.ob[0] = x;
    vertex->n.ob[1] = y;
    vertex->n.ob[2] = z;
    vertex->n.flag = 0;

    if (texture)
        vertex_set_tc(vertex, s, t, texture);

    vertex->n.n[0] = 0;
    vertex->n.n[1] = 0;
    vertex->n.n[2] = 128;

    vertex->n.a = 255;
}

static void textured_quad_set_tex_coords(fw64Mesh* mesh, int frame, float min_s, float max_s, float min_t, float max_t) {
    int index = frame * 4;

    // TODO: this needs to be changed back to textures
    vertex_set_tc(mesh->vertex_buffer + index, min_s, min_t, mesh->resources->textures);
    vertex_set_tc(mesh->vertex_buffer + index + 1, max_s, min_t, mesh->resources->textures);
    vertex_set_tc(mesh->vertex_buffer + index + 2, max_s, max_t, mesh->resources->textures);
    vertex_set_tc(mesh->vertex_buffer + index + 3, min_s, max_t, mesh->resources->textures);
}

static void create_quad_slice(fw64Mesh* mesh, int primitive_index, short tl_x, short tl_y, short size, fw64Texture* texture) {
    fw64Primitive* primitive = mesh->primitives + primitive_index;

    Vtx* vtx_base = mesh->vertex_buffer + primitive_index * 4;
    vertex_set_p_tc(vtx_base + 0, tl_x, tl_y, 0, 0.0f, 0.0f, texture);
    vertex_set_p_tc(vtx_base + 1, tl_x + size, tl_y, 0, 1.0, 0.0f, texture);
    vertex_set_p_tc(vtx_base + 2, tl_x + size, tl_y - size, 0, 1.0, 1.0, texture);
    vertex_set_p_tc(vtx_base + 3, tl_x, tl_y - size, 0, 0.0f, 1.0, texture);

    gSPVertex(mesh->display_list + primitive_index * 3, mesh->vertex_buffer + primitive_index * 4, 4, 0);
    gSP2Triangles(mesh->display_list + primitive_index * 3 + 1, 
    0, 1, 2, 0, 
    0, 2, 3, 0);
    gSPEndDisplayList(mesh->display_list + primitive_index * 3 + 2);
    
    Box slice_bounding;
    vec3_set(&slice_bounding.min, tl_x, tl_y - size, 0.0f);
    vec3_set(&slice_bounding.max, tl_x + size, tl_y, 0.0f);
    box_encapsulate_box(&mesh->info.bounding_box, &slice_bounding);

    primitive->vertices = mesh->vertex_buffer + (primitive_index * 4);
    primitive->display_list = mesh->display_list + (primitive_index * 3);
}

static fw64Mesh* make_quad_mesh(fw64TexturedQuadParams* params, fw64Allocator* allocator) {
    fw64Image* image = params->image;
    fw64Mesh* mesh = allocator->malloc(allocator, sizeof(fw64Mesh));
    fw64_n64_mesh_init(mesh);
    
    mesh->resources = allocator->malloc(allocator, sizeof(fw64MeshResources));

    // if the image is passed in then we should not delete it when the mesh is destroyed
    if (image) 
        mesh->resources->flags = FW64_MESH_FLAGS_IMAGES_ARE_SHARED;
    else
        image = fw64_image_load(NULL, params->image_asset_index, allocator);

    fw64Texture* texture = fw64_texture_create_from_image(image, allocator);

    int hslices = params->is_animated ? 1 : fw64_texture_hslices(texture);
    int vslices = params->is_animated ? 1 : fw64_texture_vslices(texture);
    uint32_t slice_count = hslices * vslices;
    
    mesh->resources->image_count = 1;
    mesh->resources->images = image;

    mesh->resources->texture_count = 1;
    mesh->resources->textures = texture;


    mesh->info.primitive_count = slice_count;
    mesh->primitives = allocator->malloc(allocator, mesh->info.primitive_count * sizeof(fw64Primitive));

    mesh->resources->material_count = slice_count;
    mesh->resources->materials = allocator->memalign(allocator, 8, mesh->info.primitive_count * sizeof(fw64Material));

    mesh->info.vertex_count = mesh->info.primitive_count * 4;
    mesh->vertex_buffer = allocator->memalign(allocator, 8, mesh->info.primitive_count * sizeof(Vtx) * 4);

    mesh->info.display_list_count = mesh->info.primitive_count * 3;
    mesh->display_list = allocator->memalign(allocator, 8, mesh->info.display_list_count * sizeof(Gfx));

    short start_x, start_y, size;

    if (slice_count == 1) {
        start_x = -1;
        start_y = 1;
        size = 2;
    }
    else {
        start_x = -hslices / 2;
        start_y = vslices / 2;
        size = 1;
    }

    short tl_x = start_x;
    short tl_y = start_y;

    int primitive_index = 0;

    box_invalidate(&mesh->info.bounding_box);
    
    for (int y = 0; y < vslices; y++) {
        for (int x = 0; x < hslices; x++) {
            create_quad_slice(mesh, primitive_index, tl_x, tl_y, size, texture);

            fw64Primitive* primitive = mesh->primitives + primitive_index;
            fw64Material* material = mesh->resources->materials + primitive_index;
            
            primitive->material = material;
            fw64_color_rgba8_set(&material->color, 255, 255, 255, 255);
            material->texture = texture;
            material->shading_mode = FW64_SHADING_MODE_UNLIT_TEXTURED;
            material->texture_frame = primitive_index;

            primitive_index += 1;
            tl_x += size;
        }

        tl_y -= size;
        tl_x = start_x;
    }

    return mesh;
}

fw64Mesh* fw64_textured_quad_create(fw64Engine* engine, int image_asset_index, fw64Allocator* allocator) {
    (void)engine;
    if (!allocator) allocator = fw64_default_allocator();

    fw64TexturedQuadParams params;
    fw64_textured_quad_params_init(&params);

    params.is_animated = 0;
    params.image_asset_index = image_asset_index;

    return make_quad_mesh(&params, allocator);
}

fw64Mesh* fw64_textured_quad_create_with_params(fw64Engine* engine, fw64TexturedQuadParams* params, fw64Allocator* allocator) {
    (void)engine;
    if (!allocator) allocator = fw64_default_allocator();

    fw64Mesh* mesh = make_quad_mesh(params, allocator);

    // This only works correctly for textures which have a single slice 
    textured_quad_set_tex_coords(mesh, 0, params->min_s, params->max_s, params->min_t, params->max_t);

    return mesh;
}

/*
fw64Mesh* quad_create(int16_t size, Color* color) {
    fw64Mesh* mesh = malloc(sizeof(fw64Mesh));
    fw64_n64_mesh_init(mesh);

    mesh->info.color_count = 1;
    mesh->colors = memalign(8, sizeof(Lights1) * 4);
    memcpy(mesh->colors->a.l.col, color, sizeof(Color));
    memcpy(mesh->colors->a.l.colc, color, sizeof(Color));

    Color light_color = {125, 125, 125};
    memcpy(mesh->colors->l[0].l.col, &light_color, sizeof(Color));
    memcpy(mesh->colors->l[0].l.colc, &light_color, sizeof(Color));

    char light_dir[3] = {40, 40, 40};
    memcpy(mesh->colors->l[0].l.dir, &light_dir[0], 3);

    mesh->info.primitive_count = 1;
    mesh->primitives = malloc(sizeof(fw64Primitive));

    mesh->info.vertex_count = 4;
    mesh->vertex_buffer = memalign(8, sizeof(Vtx) * 4);

    mesh->info.display_list_count = 3;
    mesh->display_list = memalign(8, sizeof(Gfx) * 3);

    create_quad_slice(mesh, 0, -size / 2, size / 2, size, NULL);

    fw64Primitive* primitive = mesh->primitives;

    primitive->material.color = 0;
    primitive->material.texture = NULL;
    primitive->material.texture_frame = FW64_MATERIAL_NO_TEXTURE;
    primitive->material.mode = FW64_SHADING_MODE_GOURAUD;

    return mesh;
}
*/