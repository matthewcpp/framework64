#pragma once

/** \file texture.h */

#include "framework64/allocator.h"
#include "framework64/image.h"

typedef struct fw64Texture fw64Texture;

#ifdef FW64_PLATFORM_N64_LIBULTRA
#include <nusys.h>

typedef enum {
    FW64_TEXTURE_WRAP_CLAMP = G_TX_CLAMP,
    FW64_TEXTURE_WRAP_REPEAT = G_TX_WRAP,
    FW64_TEXTURE_WRAP_MIRROR = G_TX_MIRROR
} fw64TextureWrapMode;
#else
#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

typedef enum {
    FW64_TEXTURE_WRAP_CLAMP = GL_CLAMP_TO_EDGE,
    FW64_TEXTURE_WRAP_REPEAT = GL_REPEAT,
    FW64_TEXTURE_WRAP_MIRROR =  GL_MIRRORED_REPEAT
} fw64TextureWrapMode;
#endif

#ifdef __cplusplus
extern "C" {
#endif

fw64Texture* fw64_texture_create_from_image(fw64Image* image, fw64Allocator* allocator);

/** 
 * Note this method does not delete the image!  
 * You will need to call \ref fw64_image_delete to free resources used by the underlying image 
 * */
void fw64_texture_delete(fw64Texture* texture, fw64Allocator* allocator);

int fw64_texture_width(fw64Texture* texture);
int fw64_texture_height(fw64Texture* texture);
int fw64_texture_hslices(fw64Texture* texture);
int fw64_texture_vslices(fw64Texture* texture);
int fw64_texture_slice_width(fw64Texture* texture);
int fw64_texture_slice_height(fw64Texture* texture);

void fw64_texture_set_wrap_mode(fw64Texture* texture, fw64TextureWrapMode wrap_s, fw64TextureWrapMode wrap_t);

fw64Image* fw64_texture_get_image(fw64Texture* texture);

/** 
 * Sets the image associated with this this texture.
 * This will also reset the teture's palette index to 0 (default)
 * Note, calling this method on a texture which already has an image will NOT free the resources associated with that image.
*/
void fw64_texture_set_image(fw64Texture* texture, fw64Image* image);

/** Sets the palette index to use for a color index texture. */
void fw64_texture_set_palette_index(fw64Texture* texture, uint32_t index);

/** Gets the palette index to use for a color index texture. */
uint32_t fw64_texture_get_palette_index(fw64Texture* texture);

#ifdef __cplusplus
}
#endif

