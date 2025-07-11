#include "ui.h"

#include <framework64/util/renderpass_util.h>

void ui_init(UI* ui, fw64Engine* engine, fw64Font* font, fw64Allocator* allocator) {
    ui->engine = engine;
    ui->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), allocator);
    fw64_renderpass_util_ortho2d(ui->renderpass);

    ui->spritebatch = fw64_spritebatch_create(1, allocator);

    fw64_timing_info_init(&ui->timing_info, font, engine->time);
    ivec2_set(&ui->timing_info.position, 10, 10);
}

void ui_update(UI* ui) {
    fw64_spritebatch_begin(ui->spritebatch);
    fw64_timing_info_to_spritebatch(&ui->timing_info, ui->spritebatch);
    fw64_spritebatch_end(ui->spritebatch);
}

void ui_draw(UI* ui) {
    fw64_renderpass_begin(ui->renderpass);
    fw64_renderpass_draw_sprite_batch(ui->renderpass, ui->spritebatch);
    fw64_renderpass_end(ui->renderpass);

    fw64_renderer_submit_renderpass(ui->engine->renderer, ui->renderpass);
}

void ui_uninit(UI* ui) {
    fw64_renderpass_delete(ui->renderpass);
    fw64_spritebatch_delete(ui->spritebatch);
}
