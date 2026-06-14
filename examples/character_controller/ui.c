#include "ui.h"

#include <framework64/util/renderpass_util.h>

#include <stdio.h>

static const char* get_character_state_text(fw64CharacterState state);

void ui_init(UI* ui, fw64Engine* engine, fw64Font* font, Player* player, fw64CollisionGeometryDebug* geometry_debug, fw64BumpAllocator* bump_allocator) {
    ui->engine = engine;
    ui->font = font;
    ui->geometry_debug = geometry_debug;
    ui->bump_allocator = bump_allocator;
    ui->player = player;

    ui->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), &bump_allocator->interface);
    fw64_renderpass_util_ortho2d(ui->renderpass);

    ui->spritebatch = fw64_spritebatch_create(1, &bump_allocator->interface);

    ui->position = (IVec2){10,10};
}

void ui_update(UI* ui) {
    char buffer[64];
    int line_height = fw64_font_line_height(ui->font);

    fw64_spritebatch_begin(ui->spritebatch);

    // time delta
    IVec2 draw_pos = ui->position;
    #ifdef FW64_RENDERER_DEBUG_INFO
    const fw64RendererDebugInfo* info =  fw64_renderer_get_debug_info(ui->engine->renderer);
    sprintf(buffer, "td: %.3f tri: %u", ui->engine->time->time_delta, info->triangle_count);
    #else
    sprintf(buffer, "td: %.3f", ui->engine->time->time_delta);
    #endif
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, buffer, draw_pos.x, draw_pos.y);

    // bump allocator memory
    draw_pos.y += line_height;
    const uint32_t committed = fw64_bump_allocator_committed(ui->bump_allocator);
    sprintf(buffer, "mem: %u", committed);
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, buffer, draw_pos.x, draw_pos.y);

    // character info
    draw_pos.y  += line_height;
    sprintf(buffer, "p: %.2f, %.2f, %2.f", ui->player->character.position.x, ui->player->character.position.y, ui->player->character.position.z);
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, buffer, draw_pos.x, draw_pos.y);

    draw_pos.y += line_height;
    sprintf(buffer, "v: %.2f, %.2f, %.2f", ui->player->character.velocity.x, ui->player->character.velocity.y, ui->player->character.velocity.z);
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, buffer, draw_pos.x, draw_pos.y);

    draw_pos.y += line_height;
    const char* status = get_character_state_text(ui->player->character.state);
    if (status) {
        fw64_spritebatch_draw_string(ui->spritebatch, ui->font, status, draw_pos.x, draw_pos.y);
    }

    // grid info
    IVec3 grid_pos;
    draw_pos.y += line_height;
    int on_grid = fw64_collision_geometry_get_cell_coordinates_vec3(ui->player->character.scene->collision_geometry, &ui->player->character.position, &grid_pos);
    if (on_grid) {
        sprintf(buffer, "Grid: %d,%d", grid_pos.x, grid_pos.z);
        fw64_spritebatch_draw_string(ui->spritebatch, ui->font, buffer, draw_pos.x, draw_pos.y);
    }

    // collision query info
#ifdef FW64_COLLISION_GEOMETRY_DEBUG_INFO
    const fw64CharacterEnvironmentDebugInfo* char_debug = &ui->player->character.environment->debug_info;
    draw_pos.y += fw64_font_line_height(ui->font);

    sprintf(buffer, "char: c: %d/%d/%d r:%d", 
        char_debug->capsule_triangles_considered, 
        char_debug->capsule_triangles_skipped, 
        char_debug->capsule_triangles_checked,
        char_debug->ray_triangles_checked
    );

    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, buffer, draw_pos.x, draw_pos.y);

    draw_pos.y += fw64_font_line_height(ui->font);
    sprintf(buffer, "cam: r: %d/%d/%d", ui->player->third_person_cam.collision_geometry_debug_info.triangles_considered, ui->player->third_person_cam.collision_geometry_debug_info.triangles_skipped, ui->player->third_person_cam.collision_geometry_debug_info.triangles_checked);
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, buffer, draw_pos.x, draw_pos.y);
#endif

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

const char* get_character_state_text(fw64CharacterState state) {
    char* status = NULL;

    switch (state){
        case FW64_CHARACTER_STATE_DISABLED:
            status = "Off";
            break;

        case FW64_CHARACTER_STATE_ON_GROUND:
            status = "Ground";
            break;

        case FW64_CHARACTER_STATE_IN_AIR:
            status = "Air";
            break;

        case FW64_CHARACTER_STATE_LEDGE_HANG:
            status = "Hang";
            break;

        case FW64_CHARACTER_STATE_LEDGE_CLIMB_UP:
            status = "Climb up";
            break;

        case FW64_CHARACTER_STATE_CLIMB_LADDER_UP:
            status = "Ladder Up";
            break;

        case FW64_CHARACTER_STATE_CLIMB_LADDER_DOWN:
            status = "Ladder Down";
            break;

        case FW64_CHARACTER_STATE_CLIMB_LADDER_IDLE:
            status = "Ladder Idle";
            break;

        case FW64_CHARACTER_STATE_CLIMB_LADDER_EXIT:
            status = "Ladder Exit";
            break;

        case FW64_CHARACTER_STATE_LADDER_ENTER_TOP:
            status = "Ladder Enter";
            break;
    }

    return status;
}
