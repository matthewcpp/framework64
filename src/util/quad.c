#include "framework64/util/quad.h"

#include <nusys.h>

#include <malloc.h>
#include <string.h>

static void vertex_set_tc(Vtx* vertex, float s, float t, ImageSprite* texture) {
    s *= (float)image_sprite_get_slice_width(texture) * 2.0f;
    t *= (float)image_sprite_get_slice_height(texture) * 2.0f;

    // Note that the texture coordinates (s,t) are encoded in S10.5 format.
    short ss = (short)s;
    short st = (short)t;
    vertex->n.tc[0] =  ss << 5;
    vertex->n.tc[1] = st << 5;
}

static void vertex_set_p_tc(Vtx* vertex, short x, short y, short z, float s, float t, ImageSprite* texture) {
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

void textured_quad_set_tex_coords(Mesh* mesh, int frame, float s, float t) {
    int index = frame * 4;

    vertex_set_tc(mesh->vertex_buffer + index, 0.0f, 0.0f, mesh->textures);
    vertex_set_tc(mesh->vertex_buffer + index + 1, s, 0.0f, mesh->textures);
    vertex_set_tc(mesh->vertex_buffer + index + 2, s, t, mesh->textures);
    vertex_set_tc(mesh->vertex_buffer + index + 3, 0.0f, t, mesh->textures);
}

static void create_quad_slice(Mesh* mesh, int primitive_index, short tl_x, short tl_y, short size, ImageSprite* texture) {
    Primitive* primitive = mesh->primitives + primitive_index;

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
    
    vec3_set(&primitive->bounding_box.min, tl_x, tl_y - size, 0.0f);
    vec3_set(&primitive->bounding_box.max, tl_x + size, tl_y, 0.0f);
    box_encapsulate_box(&mesh->info.bounding_box, &primitive->bounding_box);

    primitive->vertices = primitive_index * 4;
    primitive->display_list = primitive_index * 3;
}


void textured_quad_create(Mesh* mesh, ImageSprite* texture) {
    mesh_init(mesh);

    mesh->info.texture_count = 1;
    mesh->textures = texture;

    uint32_t slice_count = texture->hslices * texture->vslices;

    mesh->info.primitive_count = slice_count;
    mesh->primitives = malloc(mesh->info.primitive_count * sizeof(Primitive));

    mesh->info.vertex_count = mesh->info.primitive_count * 4;
    mesh->vertex_buffer = memalign(8, mesh->info.primitive_count * sizeof(Vtx) * 4);

    mesh->info.display_list_count = mesh->info.primitive_count * 3;
    mesh->display_list = memalign(8, mesh->info.display_list_count * sizeof(Gfx));

    short start_x, start_y, size;

    if (slice_count == 1) {
        start_x = -1;
        start_y = 1;
        size = 2;
    }
    else {
        start_x = -texture->hslices / 2;
        start_y = texture->vslices / 2;
        size = 1;
    }

    short tl_x = start_x;
    short tl_y = start_y;

    int primitive_index = 0;

    box_invalidate(&mesh->info.bounding_box);
    
    for (int y = 0; y < texture->vslices; y++) {
        for (int x = 0; x < texture->hslices; x++) {
            create_quad_slice(mesh, primitive_index, tl_x, tl_y, size, texture);

            Primitive* primitive = mesh->primitives + primitive_index;

            primitive->material.color = MATERIAL_NO_COLOR;
            primitive->material.texture = 0;
            primitive->material.texture_frame = primitive_index;
            primitive->material.mode = SHADING_MODE_UNLIT_TEXTURED;

            primitive_index += 1;
            tl_x += size;
        }

        tl_y -= size;
        tl_x = start_x;
    }
}

void quad_create(Mesh* mesh, s16 size, Color* color) {
    mesh_init(mesh);

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
    mesh->primitives = malloc(sizeof(Primitive));

    mesh->info.vertex_count = 4;
    mesh->vertex_buffer = memalign(8, sizeof(Vtx) * 4);

    mesh->info.display_list_count = 3;
    mesh->display_list = memalign(8, sizeof(Gfx) * 3);

    create_quad_slice(mesh, 0, -size / 2, size / 2, size, NULL);

    Primitive* primitive = mesh->primitives;

    primitive->material.color = 0;
    primitive->material.texture = MATERIAL_NO_TEXTURE;
    primitive->material.texture_frame = MATERIAL_NO_TEXTURE;
    primitive->material.mode = SHADING_MODE_GOURAUD;
}