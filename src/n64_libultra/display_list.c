#include "framework64/n64/display_list.h"

#include "framework64/n64/image.h"

Gfx* fw64_n64_create_quad_display_list(Gfx* display_list, Vtx* vertices, size_t count) {
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

    return display_list;
}

size_t fw64_n64_compute_quad_display_list_size(size_t count) {
    // Vertex cache can hold 32 verts -> 8 quads of 4 vertices each
    size_t total_chunk_count = count % 8 == 0 ? count / 8 : (count / 8) + 1;

    // each chunk needs 9 display list entries -> gSPVertex + 8 gSP2Triangles plus a final gSPEndDisplayList
    return total_chunk_count * 9 + 1;
}

void fw64_texture_state_default(fw64TextureState* texture_state) {
    texture_state->active_texture = NULL;
    texture_state->active_texture_frame = -1;
    texture_state->active_palette = FW64_INVALID_PALETTE_INDEX;
}

static Gfx* fw64_n64_load_indexed_texture(fw64TextureState* renderer, Gfx* display_list, fw64Texture* texture, int frame) {
    int load_texture = texture != renderer->active_texture || frame != renderer->active_texture_frame;
    int load_palette = load_texture || (texture->palette_index != renderer->active_palette);

    fw64Image* image = texture->image;

    if (renderer->active_texture == NULL|| renderer->active_texture->image->info.palette_count == 0) {
        gDPSetTextureLUT(display_list++, G_TT_RGBA16);
    }
    
    if (image->info.format == FW64_N64_IMAGE_FORMAT_CI8) {
        if (load_palette) {
            u32 palette_data = (u32)image->palettes[texture->palette_index];
            gDPLoadTLUT_pal256(display_list++, palette_data);
            renderer->active_palette = texture->palette_index;
        }
        
        if (load_texture) {
            uint8_t* image_data = fw64_n64_image_get_data(image, frame);
            int slice_width = fw64_texture_slice_width(texture);
            int slice_height = fw64_texture_slice_height(texture);

            gDPLoadTextureBlock(display_list++, image_data,
                G_IM_FMT_CI, G_IM_SIZ_8b,  slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);

            renderer->active_texture = texture;
            renderer->active_texture_frame = frame;
        }
    }
    else if (image->info.format == FW64_N64_IMAGE_FORMAT_CI4) {
        if (load_palette) {
            u32 palette_data = (u32)image->palettes[texture->palette_index];
            gDPLoadTLUT_pal16(display_list++, 0, palette_data);
            renderer->active_palette = texture->palette_index;
        }

        if (load_texture) {
            uint8_t* image_data = fw64_n64_image_get_data(image, frame);
            int slice_width = fw64_texture_slice_width(texture);
            int slice_height = fw64_texture_slice_height(texture);

            gDPLoadTextureBlock_4b(display_list++, image_data, G_IM_FMT_CI, slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);

            renderer->active_texture = texture;
            renderer->active_texture_frame = frame;
        }
    }

    return display_list;
}

static Gfx* fw64_n64_load_non_indexed_texture(fw64TextureState* renderer, Gfx* display_list, fw64Texture* texture, int frame) {
    // skip loading the same texture that we already have loaded
    if (texture == renderer->active_texture && frame == renderer->active_texture_frame && texture->palette_index == renderer->active_palette)
        return display_list;

    fw64Image* image = texture->image;
    uint8_t* image_data = fw64_n64_image_get_data(image, frame);
    int slice_width = fw64_texture_slice_width(texture);
    int slice_height = fw64_texture_slice_height(texture);

    if (renderer->active_palette != FW64_INVALID_PALETTE_INDEX) {
        gDPSetTextureLUT(display_list++, G_TT_NONE);
        renderer->active_palette = FW64_INVALID_PALETTE_INDEX;
    }

    switch (image->info.format)
    {
        case FW64_N64_IMAGE_FORMAT_RGBA16:
            gDPLoadTextureBlock(display_list++, image_data,
                G_IM_FMT_RGBA, G_IM_SIZ_16b,  slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
            break;

        case FW64_N64_IMAGE_FORMAT_RGBA32:
            gDPLoadTextureBlock(display_list++, image_data,
                G_IM_FMT_RGBA, G_IM_SIZ_32b,  slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
            break;
        
        case FW64_N64_IMAGE_FORMAT_IA8:
            gDPLoadTextureBlock(display_list++, image_data, G_IM_FMT_IA, G_IM_SIZ_8b,  slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
            break;

        case FW64_N64_IMAGE_FORMAT_I8:
            gDPLoadTextureBlock(display_list++, image_data, G_IM_FMT_I, G_IM_SIZ_8b,  slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
            break;

        case FW64_N64_IMAGE_FORMAT_IA4:
            gDPLoadTextureBlock_4b(display_list++, image_data, G_IM_FMT_IA, slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
            break;

        case FW64_N64_IMAGE_FORMAT_I4:
            gDPLoadTextureBlock_4b(display_list++, image_data, G_IM_FMT_I, slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
            break;
    }

    renderer->active_texture = texture;
    renderer->active_texture_frame = frame;
    renderer->active_palette = texture->palette_index;

    return display_list;
}

Gfx* fw64_n64_load_texture(fw64TextureState* texture_state, Gfx* display_list, fw64Texture* texture, int frame) {
    if (texture->image->info.palette_count > 0) {
        return fw64_n64_load_indexed_texture(texture_state, display_list, texture, frame);
    }
    else {
        return fw64_n64_load_non_indexed_texture(texture_state, display_list, texture, frame);
    }
}
