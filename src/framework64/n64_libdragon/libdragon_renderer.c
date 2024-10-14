#include "libdragon_renderer.h"

#include "libdragon_render_pass.h"
#include "libdragon_sprite_batch.h"
#include "libdragon_texture.h"

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64ClearFlags clear_flags) {
    // Attach and clear the screen
    renderer->disp = display_get();
    rdpq_attach_clear(renderer->disp, NULL);
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1); // colorkey (draw pixel with alpha >= 1)
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererSwapFlags swap_flags) {
    //Detach the screen
    rdpq_detach_show();
}

static void fw64_renderer_draw_sprite_batches(fw64RenderPass* renderpass) {
    for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.sprite_batches); i++){
        fw64SpriteBatch* spritebatch = *((fw64SpriteBatch**)fw64_dynamic_vector_item(&renderpass->render_queue.sprite_batches, i));

        for (size_t j = 0; j < fw64_dynamic_vector_size(&spritebatch->blit_vec); j++){
            SpriteBlitInfo* blit_info = (SpriteBlitInfo*)fw64_dynamic_vector_item(&spritebatch->blit_vec, j);
            rdpq_sprite_blit(blit_info->texture->image->libdragon_sprite, blit_info->pos.x, blit_info->pos.y, &blit_info->rdpq_params);
        }
    }
}

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    if (fw64_render_queue_has_sprite_batches(&renderpass->render_queue)) {
        fw64_renderer_draw_sprite_batches(renderpass);
    }
}