#include "framework64/desktop/screen_overlay.h"

#include "framework64/desktop/mesh_data.h"

namespace framework64 {

void ScreenOverlay::init(ShaderCache& shader_cache) {
    MeshData mesh_data;

    mesh_data.positions = {
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f
    };

    mesh_data.colors = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    mesh_data.indices_array_uint16 = {
            0, 1, 2,
            0, 2, 3
    };

    auto mesh_info = mesh_data.createGlMesh();
    mesh_info.setPrimitiveValues(primitive);
    mesh_data.moveMeshDataToPrimitive(primitive);
    primitive.mode = fw64Primitive::Mode::Triangles;
    primitive.material.color = {1.0f, 1.0f, 1.0f, 1.0f};
    primitive.material.shader = shader_cache.getShaderProgram(FW64_SHADING_MODE_VERTEX_COLOR);

}

}
