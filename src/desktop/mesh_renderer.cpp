#include "framework64/desktop/mesh_renderer.h"

#include "framework64/matrix.h"
#include "framework64/desktop/shader.h"

namespace framework64 {
bool MeshRenderer::init(std::string const & shader_dir) {
    lighting_data_uniform_block.create(0);
    mesh_transform_uniform_block.create(1);

    return true;
}

void MeshRenderer::begin(fw64Camera * cam) {
    camera = cam;

    matrix_multiply(view_projection_matrix.data(), &camera->projection.m[0], &camera->view.m[0]);

    setGlDepthTestingState();
}

void MeshRenderer::end(){
    active_shader = nullptr;
    render_mode = fw64Primitive::Mode::Unknown;
}

void MeshRenderer::updateMeshTransformBlock(fw64Transform* transform) {
    // compute the normal matrix: transpose(inverse(modelView))
    matrix_multiply(mesh_transform_uniform_block.data.normal_matrix.data(), &camera->view.m[0], &transform->matrix.m[0]);
    matrix_invert(mesh_transform_uniform_block.data.normal_matrix.data(), mesh_transform_uniform_block.data.normal_matrix.data());
    matrix_transpose(mesh_transform_uniform_block.data.normal_matrix.data());

    matrix_multiply(mesh_transform_uniform_block.data.mvp_matrix.data(), view_projection_matrix.data(), &transform->matrix.m[0]);

    mesh_transform_uniform_block.update();
}

void MeshRenderer::drawStaticMesh(fw64Transform* transform, fw64Mesh* mesh) {
    updateMeshTransformBlock(transform);

    for (auto const & primitive : mesh->primitives) {
        if (primitive.mode != render_mode)
            continue;

        setActiveShader(primitive.material.shader);
        active_shader->shader->setUniforms(primitive.material);
        glBindVertexArray(primitive.gl_vertex_array_object);

        glDrawElements(primitive.mode, primitive.element_count, primitive.element_type, 0);
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

}