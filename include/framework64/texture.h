#ifndef FW64_SPRITE_H
#define FW64_SPRITE_H

typedef struct fw64Texture fw64Texture;

#ifdef PLATFORM_N64
#include <nusys.h>

typedef enum {
    FW64_TEXTURE_WRAP_CLAMP = G_TX_CLAMP,
    FW64_TEXTURE_WRAP_REPEAT = G_TX_WRAP,
    FW64_TEXTURE_WRAP_MIRROR = G_TX_MIRROR
} fw64TextureWrapMode;
#else
#include <gl/glew.h>

typedef enum {
    FW64_TEXTURE_WRAP_CLAMP = GL_CLAMP_TO_EDGE,
    FW64_TEXTURE_WRAP_REPEAT = GL_REPEAT,
    FW64_TEXTURE_WRAP_MIRROR =  GL_MIRRORED_REPEAT
} fw64TextureWrapMode;
#endif

#ifdef __cplusplus
extern "C" {
#endif

int fw64_texture_width(fw64Texture* texture);
int fw64_texture_height(fw64Texture* texture);
int fw64_texture_hslices(fw64Texture* texture);
int fw64_texture_vslices(fw64Texture* texture);
int fw64_texture_slice_width(fw64Texture* texture);
int fw64_texture_slice_height(fw64Texture* texture);

void fw64_texture_set_wrap_mode(fw64Texture* texture, fw64TextureWrapMode wrap_s, fw64TextureWrapMode wrap_t);

#ifdef __cplusplus
}
#endif

#endif
