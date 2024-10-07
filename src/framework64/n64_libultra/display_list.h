#pragma once

#include "framework64/n64_libultra/texture.h"

#include <nusys.h>

typedef struct {
    fw64Texture* active_texture;
    int active_texture_frame;
    uint32_t active_palette;
} fw64TextureState;

void fw64_texture_state_default(fw64TextureState* texture_state);

Gfx* fw64_n64_load_texture(fw64TextureState* texture_state, Gfx* display_list, fw64Texture* texture, int frame);

/** Creates a display list that will render the vertices in the supplied array.
 * NOTE: This does not add a final gSPEndDisplayList
 */
Gfx* fw64_n64_create_quad_display_list(Gfx* display_list, Vtx* vertices, size_t count);

/** Determines the number of GFX entries required to render a number of quads */
size_t fw64_n64_compute_quad_display_list_size(size_t number_of_quads);
