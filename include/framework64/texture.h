#ifndef FW64_SPRITE_H
#define FW64_SPRITE_H

typedef struct fw64Texture fw64Texture;

#ifdef __cplusplus
extern "C" {
#endif

int fw64_texture_get_slice_width(fw64Texture *sprite);
int fw64_texture_get_slice_height(fw64Texture *sprite);

#ifdef __cplusplus
}
#endif

#endif
