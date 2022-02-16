#include "framework64/desktop/mesh_renderer.h"

#include "framework64/matrix.h"
#include "framework64/desktop/shader.h"

#include <assert.h>

namespace framework64 {
bool MeshRenderer::init(ShaderCache& shader_cache) {
    lights[0].light.light_direction = {0.57735f, 0.57735f, 0.57735f, 1.0f};
    lights[0].light.light_color = {1.0f, 1.0f, 1.0f, 1.0f};
    lights[0].active = 1;

    lights[1].light.light_direction = {-0.57735f, -0.57735f, 0.57735f, 1.0f};
    lights[1].light.light_color = {1.0f, 1.0f, 1.0f, 1.0f};
    lights[1].active = 0;

    lighting_data_uniform_block.create();
    mesh_transform_uniform_block.create();

    lighting_dirty = true;

    screen_overlay.init(shader_cache);

    return true;
}

void MeshRenderer::begin(fw64Camera * cam) {
    camera = cam;

    matrix_multiply(view_projection_matrix.data(), &camera->projection.m[0], &camera->view.m[0]);

    setGlDepthTestingState();

    if (lighting_dirty) {
        updateLightingBlock();
        lighting_dirty = false;
    }
}

void MeshRenderer::end(){
    active_shader = nullptr;
    render_mode = fw64Primitive::Mode::Unknown;
}

void MeshRenderer::updateMeshTransformBlock(fw64Matrix & matrix) {
    // compute the normal matrix: transpose(inverse(modelView))
    matrix_multiply(mesh_transform_uniform_block.data.normal_matrix.data(), &camera->view.m[0], &matrix.m[0]);
    matrix_invert(mesh_transform_uniform_block.data.normal_matrix.data(), mesh_transform_uniform_block.data.normal_matrix.data());
    matrix_transpose(mesh_transform_uniform_block.data.normal_matrix.data());

    matrix_multiply(mesh_transform_uniform_block.data.mvp_matrix.data(), view_projection_matrix.data(), &matrix.m[0]);

    mesh_transform_uniform_block.update();
}

void MeshRenderer::drawPrimitive(fw64Primitive const & primitive) {
    setActiveShader(primitive.material.shader);
    active_shader->shader->setUniforms(primitive.material);
    glBindVertexArray(primitive.gl_vertex_array_object);

    glDrawElements(primitive.mode, primitive.element_count, primitive.element_type, 0);
}

void MeshRenderer::drawStaticMesh(fw64Mesh* mesh, fw64Transform* transform) {
    updateMeshTransformBlock(transform->matrix);

    for (auto const & primitive : mesh->primitives) {
        if (primitive.mode != render_mode)
            continue;

        drawPrimitive(primitive);
    }
}

void MeshRenderer::drawAnimatedMesh(fw64Mesh *mesh, fw64AnimationController* controller, fw64Transform *transform) {
    for (auto const & primitive : mesh->primitives) {
        if (primitive.mode != render_mode)
            continue;

        // should this be done in the shader?
        fw64Matrix transform_matrix;
        matrix_multiply(&transform_matrix.m[0], &transform->matrix.m[0], &controller->matrices[primitive.joint_index].m[0]);
        updateMeshTransformBlock(transform_matrix);

        drawPrimitive(primitive);
    }
}

void MeshRenderer::setActiveShader(ShaderProgram* shader) {
    if (active_shader != shader) {
        active_shader = shader;

        glUseProgram(shader->handle);

        if (active_shader->lighting_data_uniform_block_index != GL_INVALID_INDEX)
            glUniformBlockBinding(active_shader->handle, active_shader->lighting_data_uniform_block_index, lighting_data_uniform_block.binding_index);

        if (active_shader->mesh_transform_uniform_block_index != GL_INVALID_INDEX)
            glUniformBlockBinding(active_shader->handle, active_shader->mesh_transform_uniform_block_index, mesh_transform_uniform_block.binding_index);
    }
}

void MeshRenderer::setDepthTestingEnabled(bool enabled) {
    depth_testing_enabled = enabled;

    if (render_mode == fw64Primitive::Mode::Unknown)
        return;

    setGlDepthTestingState();
}

void MeshRenderer::setGlDepthTestingState() {
    if (depth_testing_enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void MeshRenderer::updateLightingBlock() {
    int block_index = 0;

    for (auto const & light_info : lights) {
        if (light_info.active) {
            lighting_data_uniform_block.data.lights[block_index++] = light_info.light;
        }
    }

    lighting_data_uniform_block.data.light_count = block_index;
    lighting_data_uniform_block.update();
}

void MeshRenderer::setAmbientLightColor(uint8_t r, uint8_t g, uint8_t b) {
    lighting_data_uniform_block.data.ambient_light_color[0] = static_cast<float>(r) / 255.0f;
    lighting_data_uniform_block.data.ambient_light_color[1] = static_cast<float>(r) / 255.0f;
    lighting_data_uniform_block.data.ambient_light_color[2] = static_cast<float>(r) / 255.0f;

    lighting_dirty = true;
}

void MeshRenderer::setLightEnabled(int index, int enabled) {
    assert(index < FW64_RENDERER_MAX_LIGHT_COUNT);

    lights[index].active = enabled;

    lighting_dirty = true;
}

void MeshRenderer::setLightDirection(int index, float x, float y, float z) {
    assert(index < FW64_RENDERER_MAX_LIGHT_COUNT);

    lights[index].light.light_direction[0] = x;
    lights[index].light.light_direction[1] = y;
    lights[index].light.light_direction[2] = z;

    lighting_dirty = true;
}

void MeshRenderer::setLightColor(int index, uint8_t r, uint8_t g, uint8_t b) {
    assert(index < FW64_RENDERER_MAX_LIGHT_COUNT);

    lights[index].light.light_color[0] = static_cast<float>(r) / 255.0f;
    lights[index].light.light_color[1] = static_cast<float>(g) / 255.0f;
    lights[index].light.light_color[2] = static_cast<float>(b) / 255.0f;

    lighting_dirty = true;
}

void MeshRenderer::renderFullscreenOverlay(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    screen_overlay.primitive.material.color[0] = static_cast<float>(r) / 255.0f;
    screen_overlay.primitive.material.color[1] = static_cast<float>(g) / 255.0f;
    screen_overlay.primitive.material.color[2] = static_cast<float>(b) / 255.0f;
    screen_overlay.primitive.material.color[3] = static_cast<float>(a) / 255.0f;

    matrix_set_identity(mesh_transform_uniform_block.data.normal_matrix.data());
    matrix_set_identity(mesh_transform_uniform_block.data.mvp_matrix.data());
    mesh_transform_uniform_block.update();

    glDisable(GL_DEPTH_TEST);
    drawPrimitive(screen_overlay.primitive);
}

}