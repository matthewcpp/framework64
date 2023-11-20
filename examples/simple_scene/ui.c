#include "ui.h"

#include "assets/assets.h"

#include <stdio.h>

void ui_init(UI* ui, fw64Engine* engine, Player* player) {
    ui->engine = engine;
    ui->player = player;

    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, fw64_default_allocator());
}

void ui_update(UI* ui) {
    Vec3* pos = &ui->player->node.transform.position;
    const char* status_text;

    switch (ui->player->state) {
        case PLAYER_STATE_ON_GROUND:
            status_text = "Ground";
            break;

        case PLAYER_STATE_FALLING:
            status_text = "Falling";
            break;
    }

    sprintf(ui->status_text, "%.3f %.3f %.3f %s %.3f", pos->x, pos->y, pos->z, status_text, ui->player->air_velocity);
}

void ui_draw(UI* ui) {
    fw64_renderer_draw_text(ui->engine->renderer, ui->font, 10,10, ui->status_text);
}