#include "ui.h"

#include "assets/assets.h"

#include "framework64/util/renderpass_util.h"

#include <stdio.h>

void ui_init(UI* ui, fw64Engine* engine, Player* player) {
    fw64Allocator* allocator = fw64_default_allocator();

    ui->engine = engine;
    ui->player = player;

    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui->spritebatch = fw64_spritebatch_create(1, allocator);
}

void ui_update(UI* ui) {
    char status_text[64];
    Vec3* pos = &ui->player->node->transform.position;
    const char* state = "";

    switch (ui->player->state) {
        case PLAYER_STATE_ON_GROUND:
            state = "Ground";
            break;

        case PLAYER_STATE_FALLING:
            state = "Falling";
            break;
    }

    sprintf(status_text, "%.3f %.3f %.3f %s %.3f", pos->x, pos->y, pos->z, state, ui->player->air_velocity);

    fw64_spritebatch_begin(ui->spritebatch);
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, status_text, 10, 10);
    fw64_spritebatch_end(ui->spritebatch);
}
