#include "framework64/util/quad.h"
#include "framework64/desktop/texture.h"
#include "framework64/vec2.h"
#include "framework64/desktop/mesh.h"

fw64Mesh* textured_quad_create(fw64Texture* texture) {
    uint32_t slice_count = texture->hslices * texture->vslices;

    Vec2 top_left = {-1.0f, 1.0f};
    Vec2 bottom_right = {1.0f, -1.0f};

    if(slice_count > 1) {
        top_left = {-texture->hslices / 2.0f, texture->vslices / 2.0f};
        bottom_right = {texture->hslices / 2.0f, -texture->vslices / 2.0f};
    }

    auto* mesh = new fw64Mesh();

    return mesh;
}

void textured_quad_set_tex_coords(fw64Mesh* mesh, int frame, float s, float t) {

}

fw64Mesh* quad_create(int16_t size, Color* color) {

}