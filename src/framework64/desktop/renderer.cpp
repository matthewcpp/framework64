#include "framework64/desktop/renderer.hpp"

#include "framework64/desktop/render_pass.hpp"

#include "framework64/matrix.h"
#include "framework64/node.h"

#include <cassert>
#include <iostream>

bool fw64Renderer::init(int width, int height) {
    if (!initFramebuffer(width, height)) {
        return false;
    }

    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    sprite_material.texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
    sprite_material.shader = shader_cache.getShaderProgram(FW64_SHADING_MODE_UNLIT_TRANSPARENT_TEXTURED);
    sprite_material.shading_mode = FW64_SHADING_MODE_UNLIT_TRANSPARENT_TEXTURED;

    fog_data_uniform_block.create();
    lighting_data_uniform_block.create();
    mesh_transform_uniform_block.create();

    return true;
}

bool fw64Renderer::initFramebuffer(int width, int height) {
    if (!framebuffer.initialize(width, height)) {
        std::cout << "Failed to initialize framebuffer" << std::endl;
        return false;
    }
    return true;
}

void fw64Renderer::setClearColor(float r, float g, float b, float a) {
    clear_color = {r, g, b, a};
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
}

void fw64Renderer::beginFrame() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.framebuffer_handle);
}

void fw64Renderer::endFrame() {
    renderpass_index = 0;

    for (auto* renderpass : renderpasses) {
        drawRenderPass(renderpass);
        renderpass_index += 1;
    }

    current_camera = nullptr;
    active_shader = nullptr;

    renderpasses.clear();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.framebuffer_handle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(  0, 0, framebuffer.width, framebuffer.height,
                        0, 0, display.window_width(), display.window_height(),
                        GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    display.swap();
}

fw64Renderer::ViewportRect fw64Renderer::getViewportRect(fw64Viewport const * viewport) const {
    float bottom = static_cast<float>(viewport->position.y + viewport->size.y);

    return {
        static_cast<GLint>(viewport->position.x ),
        static_cast<GLint>((framebuffer.height - bottom)),
        static_cast<GLsizei>(viewport->size.x),
        static_cast<GLsizei>(viewport->size.y)
    };
}

void fw64Renderer::setCamera(fw64Camera* camera) {
    if (camera == current_camera)
        return;

    mesh_transform_uniform_block.data.camera_near = camera->near;
    mesh_transform_uniform_block.data.camera_far = camera->far;

    current_camera = camera;

    setViewport(&camera->viewport);
    setViewMatrices(camera->projection.m, camera->view.m);
}

void fw64Renderer::setViewport(fw64Viewport const * viewport) {
    auto viewport_rect = getViewportRect(viewport);
    glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
}

void fw64Renderer::setViewMatrices(float* projection, float* view) {
    std::memcpy(projection_matrix.data(), projection, sizeof(float) * 16);
    std::memcpy(view_matrix.data(), view, sizeof(float) * 16);
    matrix_multiply(projection, view, view_projection_matrix.data());
}

void fw64Renderer::clearViewport(fw64Viewport const & viewport, fw64ClearFlags flags) {
    glEnable(GL_SCISSOR_TEST);

    auto viewport_rect = getViewportRect(&viewport);
    glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
    glScissor(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);

    GLbitfield clear_flags = 0;
    if (flags & FW64_CLEAR_FLAG_COLOR)
        clear_flags |= GL_COLOR_BUFFER_BIT;

    if (flags & FW64_CLEAR_FLAG_DEPTH)
        clear_flags |= GL_DEPTH_BUFFER_BIT;

    glClear(clear_flags);

    glDisable(GL_SCISSOR_TEST);

    // restore the camera viewport if necessary
    if (current_camera) {
        viewport_rect = getViewportRect(&current_camera->viewport);
        glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
    }
}

static inline fw64PrimitiveMode getFw64PrimitiveMode(fw64Primitive::Mode mode) {
    return mode == fw64Primitive::Mode::Lines ? FW64_PRIMITIVE_MODE_LINES : FW64_PRIMITIVE_MODE_TRIANGLES;
}

void fw64Renderer::drawMeshesFromQueue(fw64RenderPass* renderpass, fw64ShadingMode index) {
    fw64DynamicVector* queue = &renderpass->render_queue.buckets[index].static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);

        updateMeshTransformBlock(draw_info->instance->node->transform.world_matrix);
        const auto& primitive = draw_info->instance->mesh->primitives[draw_info->index];
        const fw64Material* material = fw64_material_collection_get_material(draw_info->instance->materials, draw_info->index);

        if (getFw64PrimitiveMode(primitive.mode) != renderpass->primitive_mode) {
            continue;
        }

        drawPrimitive(primitive, material);
    }

    queue = &renderpass->render_queue.buckets[index].skinned_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64SkinnedDrawInfo* draw_info = (fw64SkinnedDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64SkinnedMeshInstance* skinned_mesh_instance = draw_info->instance;

        const auto & primitive = skinned_mesh_instance->skinned_mesh->mesh->primitives[draw_info->index];
        const fw64Material* material = fw64_material_collection_get_material(skinned_mesh_instance->mesh_instance.materials, draw_info->index);

        if (getFw64PrimitiveMode(primitive.mode) != renderpass->primitive_mode) {
            continue;
        }


        updateMeshTransformBlock(&skinned_mesh_instance->controller.matrices[primitive.joint_index].m[0]);
        drawPrimitive(primitive, material);
    }
}

void fw64Renderer::drawRenderPass(fw64RenderPass* renderpass) {
    fw64ClearFlags clear_flags = renderpass->clear_flags;
    if (clear_flags == FW64_CLEAR_FLAG_DEFAULT) {
        clear_flags = renderpass_index == 0 ? FW64_CLEAR_FLAG_ALL : FW64_CLEAR_FLAG_NONE;
    }
    if (clear_flags) {
        glClearColor(renderpass->clear_color[0], renderpass->clear_color[1], renderpass->clear_color[2], 1.0f);
        clearViewport(renderpass->viewport, clear_flags);

        // TODO: remove this call when clear_color api is removed
        glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
    }

    setViewport(&renderpass->viewport);
    setViewMatrices(renderpass->projection_matrix.data(), renderpass->view_matrix.data());
    mesh_transform_uniform_block.data.camera_near = renderpass->camera_near;
    mesh_transform_uniform_block.data.camera_far = renderpass->camera_far;

    if (renderpass->depth_testing_enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    if (renderpass->fog_enabled) {
        setFogEnabled(renderpass->fog_enabled);
        setFogPositions(renderpass->fog_begin, renderpass->fog_end);
        setFogColor(renderpass->fog_color[0], renderpass->fog_color[1], renderpass->fog_color[2]);
        fog_data_uniform_block.update();
    } else if (fog_enabled) {
        setFogEnabled(false);
        fog_data_uniform_block.update();
    }

    if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_LIT) || 
        fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_LIT_TEXTURED)) {
        updateLightingBlock(renderpass->lighting_info);
    }

    drawMeshesFromQueue(renderpass, FW64_SHADING_MODE_UNLIT);
    drawMeshesFromQueue(renderpass, FW64_SHADING_MODE_UNLIT_TEXTURED);
    drawMeshesFromQueue(renderpass, FW64_SHADING_MODE_LIT);
    drawMeshesFromQueue(renderpass, FW64_SHADING_MODE_LIT_TEXTURED);
    drawMeshesFromQueue(renderpass, FW64_SHADING_MODE_DECAL_TEXTURE);
    drawMeshesFromQueue(renderpass, FW64_SHADING_MODE_LINE);
    drawMeshesFromQueue(renderpass, FW64_SHADING_MODE_UNLIT_TRANSPARENT_TEXTURED);

    if (!fw64_dynamic_vector_is_empty(&renderpass->render_queue.sprite_batches)) {
        glDisable(GL_DEPTH_TEST);

        for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.sprite_batches); i++){
            fw64SpriteBatch* sprite_batch = *((fw64SpriteBatch**)fw64_dynamic_vector_item(&renderpass->render_queue.sprite_batches, i));
            drawSpriteBatch(sprite_batch);
        }
    }
}

void fw64Renderer::drawSpriteBatch(fw64SpriteBatch* sprite_batch) {
    float matrix[16];
    matrix_set_identity(matrix);

    updateMeshTransformBlock(matrix);
    setActiveShader(sprite_material.shader);

    for (const auto & layer : sprite_batch->layers) {
        for (const auto & batch : layer.batches) {
            sprite_material.texture = batch.first;
            active_shader->shader->setUniforms(active_shader, sprite_material);

            auto& sprite_vertex_buffer = sprite_batch->vertex_buffers[batch.second];
            glBindVertexArray(sprite_vertex_buffer.gl_vertex_array_object);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(sprite_vertex_buffer.cpu_buffer.size()));
            glBindVertexArray(0);
        }
    }
}

void fw64Renderer::updateMeshTransformBlock(float* matrix) {
    // compute the normal matrix: transpose(inverse(modelView))
    matrix_multiply(view_matrix.data(), matrix, mesh_transform_uniform_block.data.normal_matrix.data());
    matrix_invert(mesh_transform_uniform_block.data.normal_matrix.data(), mesh_transform_uniform_block.data.normal_matrix.data());
    matrix_transpose(mesh_transform_uniform_block.data.normal_matrix.data());

    matrix_multiply(view_projection_matrix.data(), matrix, mesh_transform_uniform_block.data.mvp_matrix.data());

    mesh_transform_uniform_block.update();
}

void fw64Renderer::drawPrimitive(fw64Primitive const & primitive, const fw64Material* material) {
    setActiveShader(material->shader);
    active_shader->shader->setUniforms(active_shader, *material);
    glBindVertexArray(primitive.gl_info.gl_vertex_array_object);

    glDrawElements(static_cast<GLenum>(primitive.mode), primitive.gl_info.element_count, primitive.gl_info.primitive_mode, 0);
}

void fw64Renderer::setActiveShader(framework64::ShaderProgram* shader) {
    if (active_shader != shader) {
        active_shader = shader;

        glUseProgram(active_shader->handle);

        if (active_shader->lighting_data_uniform_block_index != GL_INVALID_INDEX)
            glUniformBlockBinding(active_shader->handle, active_shader->lighting_data_uniform_block_index, lighting_data_uniform_block.binding_index);

        if (active_shader->mesh_transform_uniform_block_index != GL_INVALID_INDEX)
            glUniformBlockBinding(active_shader->handle, active_shader->mesh_transform_uniform_block_index, mesh_transform_uniform_block.binding_index);

        if (active_shader->fog_uniform_block_index != GL_INVALID_INDEX)
            glUniformBlockBinding(active_shader->handle, active_shader->fog_uniform_block_index, fog_data_uniform_block.binding_index);
    }
}

void fw64Renderer::updateLightingBlock(const LightingInfo& lighting_info) {
    int block_index = 0;

    lighting_data_uniform_block.data.ambient_light_color = lighting_info.ambient_color;

    for (auto const & light_info : lighting_info.lights) {
        if (!light_info.active) {
            continue;
        }

        // Shader expects to receive data in the form of the direction TO the light in camera space
        lighting_data_uniform_block.data.lights[block_index]= light_info.light;
        Vec3* light_dir = reinterpret_cast<Vec3*>(lighting_data_uniform_block.data.lights[block_index].direction.data());
        
        float mat3[9];
        mat3_set_from_mat4(mat3, view_matrix.data());
        mat3_transform_vec3(mat3, light_dir);
        vec3_normalize(light_dir);
        vec3_negate(light_dir);
        block_index += 1;
    }

    lighting_data_uniform_block.data.light_count = block_index;
    lighting_data_uniform_block.update();
}

void fw64Renderer::setFogEnabled(bool enabled) {
    fog_enabled = enabled;

    if (enabled) {
        setFogPositions(fog_min_distance, fog_max_distance);
    } else {
        setFogPositions(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    }
}

void fw64Renderer::setFogPositions(float fog_min, float fog_max) {
    fog_min_distance = fog_min;
    fog_max_distance = fog_max;
    fog_data_uniform_block.data.min_distance = fog_min;
    fog_data_uniform_block.data.max_distance = fog_max;
}

void fw64Renderer::setFogColor(float r, float g, float b) {
    fog_data_uniform_block.data.fog_color[0] = r;
    fog_data_uniform_block.data.fog_color[1] = g;
    fog_data_uniform_block.data.fog_color[2] = b;
}

void fw64Renderer::submitRenderpass(fw64RenderPass* renderpass) {
    renderpasses.push_back(renderpass);
}

// Public C interface

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->setClearColor( r / 255.0f, g / 255.0f, b /255.0f, 1.0f);
}

void fw64_renderer_set_view_matrices(fw64Renderer* renderer, fw64Matrix* projection, uint16_t*, fw64Matrix* view) {
    renderer->setViewMatrices(projection->m, view->m);
}

void fw64_renderer_set_viewport(fw64Renderer* renderer, fw64Viewport* viewport) {
    renderer->setViewport(viewport);
}

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    renderer->submitRenderpass(renderpass);
}
