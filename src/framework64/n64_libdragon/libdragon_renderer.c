#include "libdragon_renderer.h"

#include "libdragon_mesh.h"
#include "libdragon_render_pass.h"
#include "libdragon_sprite_batch.h"
#include "libdragon_texture.h"
#include "libdragon_vertex.h"

#include "framework64/vec4.h"

#include <GL/gl_integration.h>

void fw64_libdragon_renderer_new_frame(fw64Renderer* renderer) {
    renderer->renderpass_count = 0;
}

static void fw64_renderer_draw_sprite_batches(fw64RenderPass* renderpass) {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexHalfFixedPrecisionN64(FW64_LIBDRAGON_FRACTIONAL_BITS);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordHalfFixedPrecisionN64(FW64_LIBDRAGON_FRACTIONAL_BITS);
    glEnableClientState(GL_COLOR_ARRAY);

    for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.sprite_batches); i++){
        fw64SpriteBatch* spritebatch = *((fw64SpriteBatch**)fw64_dynamic_vector_item(&renderpass->render_queue.sprite_batches, i));

        for (size_t l = 0; l < spritebatch->layer_count; l++) {
            fw64SpriteBatchLayer* layer = spritebatch->layers + l;

            for (size_t b = 0; b < fw64_dynamic_vector_size(&layer->batches); b++) {
                fw64SpriteBatchTexture* texture = *((fw64SpriteBatchTexture**)fw64_dynamic_vector_item(&layer->batches, b));
                fw64LibdragonSpriteSlice* sprite = texture->id.tex_info.texture->image->sprites + texture->id.tex_info.index;
                glBindTexture(GL_TEXTURE_2D, sprite->gl_handle);
                
                fw64SpritebatchChunk* chunk = texture->first;
                while (chunk) {
                    Fw64LibdragonSpriteVertex* vertices = (Fw64LibdragonSpriteVertex*)fw64_static_vector_data(&chunk->vertices);
                    glVertexPointer(3, GL_HALF_FIXED_N64, sizeof(Fw64LibdragonSpriteVertex), (void*)(&vertices->pos[0]));
                    glTexCoordPointer(2, GL_HALF_FIXED_N64, sizeof(Fw64LibdragonSpriteVertex), (void*)(&vertices->tex_coord[0]));
                    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Fw64LibdragonSpriteVertex), (void*)(&vertices->color[0]));
                    glDrawArrays(GL_TRIANGLES, 0, fw64_static_vector_size(&chunk->vertices));

                    chunk = chunk->next;
                }
            }
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
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
        glBindTexture(GL_TEXTURE_2D, material->texture->image->sprites->gl_handle);

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
        glBindTexture(GL_TEXTURE_2D, material->texture->image->sprites->gl_handle);

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

static GLbitfield get_clear_flags(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    fw64ClearFlags clear_flags = renderpass->clear_flags;
    if (clear_flags == FW64_CLEAR_FLAG_DEFAULT) {
        clear_flags = renderer->renderpass_count == 0 ? FW64_CLEAR_FLAG_ALL : FW64_CLEAR_FLAG_NONE;
    }

    GLbitfield result = 0;

    if (clear_flags & FW64_CLEAR_FLAG_COLOR) {
        result |= GL_COLOR_BUFFER_BIT;
    }
    if (clear_flags & FW64_CLEAR_FLAG_DEPTH) {
        result |= GL_DEPTH_BUFFER_BIT;
    }

    return result;
}

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    gl_context_begin();

    // rdpq_set_mode_standard();
    // rdpq_mode_alphacompare(1); /* colorkey (draw pixel with alpha >= 1) */

    glClearColor(renderpass->clear_color.x, renderpass->clear_color.y, renderpass->clear_color.z, renderpass->clear_color.w);
    glViewport(renderpass->viewport.position.x, display_get_height() - (renderpass->viewport.position.y + renderpass->viewport.size.y), renderpass->viewport.size.x, renderpass->viewport.size.y);
    glClear(get_clear_flags(renderer, renderpass));

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

    gl_context_end();

    renderer->renderpass_count += 1;
}
