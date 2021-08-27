#ifndef FW64_SPRITE_H
#define FW64_SPRITE_H

typedef struct fw64Texture fw64Texture;

#ifdef __cplusplus
extern "C" {
#endif

int fw64_texture_width(fw64Texture* texture);
int fw64_texture_height(fw64Texture* texture);
int fw64_texture_hslices(fw64Texture* texture);
int fw64_texture_vslices(fw64Texture* texture);
int fw64_texture_slice_width(fw64Texture* texture);
int fw64_texture_slice_height(fw64Texture* texture);

#ifdef __cplusplus
}
#endif

#endif
