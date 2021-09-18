#include "framework64/desktop/material.h"

// C Interface

fw64Texture* fw64_material_get_texture(fw64Material* material) {
    return material->texture;
}