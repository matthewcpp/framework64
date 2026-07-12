#include "libdragon_material.h"

fw64ShadingMode fw64_material_get_shading_mode(const fw64Material* material) {
    return material->shading_mode;
}