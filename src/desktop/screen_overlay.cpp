#include "framework64/desktop/screen_overlay.h"

#include "framework64/desktop/primitive_data.h"

namespace framework64 {

void ScreenOverlay::init(ShaderCache& shader_cache) {
    PrimitiveData mesh_data;

    primitive.primitive_data.positions = {
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f
    };

    primitive.primitive_data.colors = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    primitive.primitive_data.indices_array_uint16 = {
            0, 1, 2,
            0, 2, 3
    };

    primitive.gl_info = mesh_data.createGlMesh();
    primitive.mode = fw64Primitive::Mode::Triangles;
    primitive.material = &material;
    material.color = {1.0f, 1.0f, 1.0f, 1.0f};
    material.shader = shader_cache.getShaderProgram(FW64_SHADING_MODE_VERTEX_COLOR);

}

}
