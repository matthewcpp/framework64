#ifndef FW64_MATERIAL_H
#define FW64_MATERIAL_H

#include "framework64/texture.h"

typedef struct fw64Material fw64Material;

#ifdef __cplusplus
extern "C" {
#endif

fw64Texture* fw64_material_get_texture(fw64Material* material);

#ifdef __cplusplus
}
#endif


#endif
