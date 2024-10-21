#include "libdragon_renderer.h"

#include "libdragon_mesh.h"
#include "libdragon_render_pass.h"
#include "libdragon_sprite_batch.h"
#include "libdragon_texture.h"

#include "framework64/vec4.h"

#include <GL/gl_integration.h>

void fw64_libdragon_renderer_init(fw64Renderer* renderer) {
    fw64_renderer_set_clear_color(renderer, 0, 0, 0);
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64ClearFlags clear_flags) {
    // rdpq_set_mode_standard();
    // rdpq_mode_alphacompare(1); /* colorkey (draw pixel with alpha >= 1) */

    gl_context_begin();

    glClearColor(renderer->clear_color[0], renderer->clear_color[1], renderer->clear_color[2], renderer->clear_color[3]);
    glClear((GLbitfield)clear_flags);
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererSwapFlags swap_flags) {
    gl_context_end();
    
}

static float quad[] = {
    1.0f, 1.0f, 0.0f,      1.0f, 1.0f,
    0.0f, 1.0f, 0.0f,      0.0f, 1.0f,
    0.0f, 0.0f, 0.0f,      0.0f, 0.0f,

    1.0f, 1.0f, 0.0f,      1.0f, 1.0f,
    0.0f, 0.0f, 0.0f,      0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,      1.0f, 0.0f,
};

#define VERTEX_STRIDE sizeof(float) * 5
#define VERTEX_COUNT 6

static void fw64_renderer_draw_sprite_batches(fw64RenderPass* renderpass) {
    static float* vertices = NULL;
    if (!vertices) {
        vertices = fw64_allocator_memalign(fw64_default_allocator(), 16, VERTEX_STRIDE * VERTEX_COUNT);
        memcpy(vertices, &quad[0], VERTEX_STRIDE * VERTEX_COUNT);
    }
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, VERTEX_STRIDE, (void*)(&vertices[0]));
    glTexCoordPointer(2, GL_FLOAT, VERTEX_STRIDE, (void*)(&vertices[3]));

    for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.sprite_batches); i++){
        fw64SpriteBatch* spritebatch = *((fw64SpriteBatch**)fw64_dynamic_vector_item(&renderpass->render_queue.sprite_batches, i));

        for (size_t j = 0; j < fw64_dynamic_vector_size(&spritebatch->blit_vec); j++){
            SpriteBlitInfo* draw_info = (SpriteBlitInfo*)fw64_dynamic_vector_item(&spritebatch->blit_vec, j);
            glBindTexture(GL_TEXTURE_2D, draw_info->texture->image->gl_handle);
            glPushMatrix();
            glTranslatef(draw_info->pos.x, draw_info->pos.y, 0.0f);
            glScalef(32.0f, 32.0f, 0.0f);
            glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT);
            glPopMatrix();
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

static void fw64_renderer_draw_unlit(fw64RenderPass* renderpass) {
    fw64RenderQueueBucket* render_queue_bucket = fw64_render_queue_get_bucket(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT);
    fw64DynamicVector* queue = &render_queue_bucket->static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);
        //fw64Material* material = fw64_material_collection_get_material(draw_info->instance->materials, draw_info->index);
        fw64Primitive* primitive = draw_info->instance->mesh->primitives + draw_info->index;

        glVertexPointer(3, GL_FLOAT, sizeof(fw64UnlitVert), (void*)primitive->vertex_data);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(fw64UnlitVert), (void*)(primitive->vertex_data + 3));

        glPushMatrix();
        glMultMatrixf(draw_info->instance->node->transform.world_matrix);
        glDrawElements(primitive->info.mode, primitive->info.element_count, GL_UNSIGNED_SHORT, primitive->element_data);
        glPopMatrix();
    }
}

static void fw64_renderer_draw_unlit_textured(fw64RenderPass* renderpass) {
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    fw64RenderQueueBucket* render_queue_bucket = fw64_render_queue_get_bucket(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT_TEXTURED);
    fw64DynamicVector* queue = &render_queue_bucket->static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64Material* material = fw64_material_collection_get_material(draw_info->instance->materials, draw_info->index);
        fw64Primitive* primitive = draw_info->instance->mesh->primitives + draw_info->index;

        glVertexPointer(3, GL_FLOAT, sizeof(fw64UnlitTexturedVert), (void*)primitive->vertex_data);
        glTexCoordPointer(2, GL_FLOAT, sizeof(fw64UnlitTexturedVert), (void*)(primitive->vertex_data + 3));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(fw64UnlitTexturedVert), (void*)(primitive->vertex_data + 5));
        glBindTexture(GL_TEXTURE_2D, material->texture->image->gl_handle);

        glPushMatrix();
        glMultMatrixf(draw_info->instance->node->transform.world_matrix);
        glDrawElements(primitive->info.mode, primitive->info.element_count, GL_UNSIGNED_SHORT, primitive->element_data);
        glPopMatrix();
    }

    queue =  &render_queue_bucket->skinned_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64SkinnedDrawInfo* draw_info = (fw64SkinnedDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64SkinnedMeshInstance* skinned_mesh_instance = draw_info->instance;
        fw64AnimationController* controller = &skinned_mesh_instance->controller;
        const fw64Primitive* primitive = &skinned_mesh_instance->skinned_mesh->mesh->primitives[draw_info->index];
        const fw64Material* material = fw64_material_collection_get_material(skinned_mesh_instance->mesh_instance.materials, draw_info->index);
 
        glVertexPointer(3, GL_FLOAT, sizeof(fw64UnlitTexturedVert), (void*)primitive->vertex_data);
        glTexCoordPointer(2, GL_FLOAT, sizeof(fw64UnlitTexturedVert), (void*)(primitive->vertex_data + 3));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(fw64UnlitTexturedVert), (void*)(primitive->vertex_data + 5));
        glBindTexture(GL_TEXTURE_2D, material->texture->image->gl_handle);

        glPushMatrix();
        glMultMatrixf(skinned_mesh_instance->mesh_instance.node->transform.world_matrix);
        glMultMatrixf(controller->matrices[primitive->info.joint_index].m);
        glDrawElements(primitive->info.mode, primitive->info.element_count, GL_UNSIGNED_SHORT, primitive->element_data);
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

static void fw64_renderer_draw_lit(fw64RenderPass* renderpass) {
    fw64RenderQueueBucket* render_queue_bucket = fw64_render_queue_get_bucket(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT);
    fw64DynamicVector* queue = &render_queue_bucket->static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);
        //fw64Material* material = fw64_material_collection_get_material(draw_info->instance->materials, draw_info->index);
        fw64Primitive* primitive = draw_info->instance->mesh->primitives + draw_info->index;

        GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);

        glVertexPointer(3, GL_FLOAT, sizeof(fw64LitVert), (void*)primitive->vertex_data);
        glNormalPointer(GL_FLOAT, sizeof(fw64LitVert), (void*)(primitive->vertex_data + 3));

        glPushMatrix();
        glMultMatrixf(draw_info->instance->node->transform.world_matrix);
        glDrawElements(primitive->info.mode, primitive->info.element_count, GL_UNSIGNED_SHORT, primitive->element_data);
        glPopMatrix();
    }
}

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    glViewport(renderpass->viewport.position.x, display_get_height() - (renderpass->viewport.position.y + renderpass->viewport.size.y), renderpass->viewport.size.x, renderpass->viewport.size.y);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(renderpass->projection_matrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(renderpass->view_matrix);

    if (renderpass->render_features & FW64_LIBDRAGON_RENDERPASS_FEATURE_DEPTH_TESTING) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT)) {
        fw64_renderer_draw_unlit(renderpass);
    }

    if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT_TEXTURED)) {
        fw64_renderer_draw_unlit_textured(renderpass);
    }

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    if (fw64_render_queue_has_sprite_batches(&renderpass->render_queue)) {
        fw64_renderer_draw_sprite_batches(renderpass);
    }
}

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    vec4_set((Vec4*)&renderer->clear_color[0], (GLfloat)r / 255.0f, (GLfloat)g / 255.0f, (GLfloat)b / 255.0f, 1.0f);
}