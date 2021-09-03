#include "framework64/desktop/mesh_renderer.h"

#include "framework64/matrix.h"
#include "framework64/desktop/shader.h"

namespace framework64 {
bool MeshRenderer::init(std::string const & shader_dir) {
    lighting_data_uniform_block.create(0);
    mesh_transform_uniform_block.create(1);

    return gouraud_shader.create(shader_dir);
}

void MeshRenderer::begin(fw64Camera * cam) {
    camera = cam;

    matrix_multiply(view_projection_matrix.data(), &camera->projection.m[0], &camera->view.m[0]);
}

void MeshRenderer::end(){
    active_program = 0;
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
        setActiveProgram(primitive.material.shader_program);
        gouraud_shader.setUniforms(primitive.material); //TODO this should be dependant on the shader referenced by the material
        glBindVertexArray(primitive.gl_vertex_array_object);

        glDrawElements(GL_TRIANGLES, primitive.element_count, GL_UNSIGNED_SHORT, 0);
    }
}

void MeshRenderer::setActiveProgram(GLuint program) {
    if (active_program != program) {
        active_program = program;

        glUseProgram(active_program);

        glUniformBlockBinding(active_program, gouraud_shader.lighting_data_uniform_block_index, lighting_data_uniform_block.binding_index);
        glUniformBlockBinding(active_program, gouraud_shader.mesh_transform_uniform_block_index, mesh_transform_uniform_block.binding_index);
    }
}

//TODO this needs to look at the mode / material to determine the correct shader program
void MeshRenderer::setupMesh(fw64Mesh* mesh) {
    for (auto & primitive : mesh->primitives) {
        primitive.material.shader_program = gouraud_shader.program;
    }
}

bool MeshRenderer::GouraudShader::create(std::string const& shader_dir) {
    std::string vertex_path = shader_dir + "gouraud.vert.glsl";
    std::string fragment_path = shader_dir + "gouraud.frag.glsl";

    program = Shader::createFromPaths(vertex_path, fragment_path);

    if (!program)
        return false;

    lighting_data_uniform_block_index = glGetUniformBlockIndex(program, "fw64LightingData");
    mesh_transform_uniform_block_index = glGetUniformBlockIndex(program, "fw64MeshTransformData");
    diffuse_color_location = glGetUniformLocation(program, "diffuse_color");

    return  lighting_data_uniform_block_index != GL_INVALID_INDEX &&
            mesh_transform_uniform_block_index != GL_INVALID_INDEX &&
            diffuse_color_location != -1;
}

void MeshRenderer::GouraudShader::setUniforms(fw64Mesh::Material const & material) {
    glUniform4fv(diffuse_color_location, 1, material.color.data());
}

}