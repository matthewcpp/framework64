#include "game.h"
#include "assets.h"

#include "framework64/n64/controller_button.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_fps_camera_init(&game->fps_camera, engine->input);
    vec3_set(&game->fps_camera.camera.transform.position, -167.0f, 31.0f, 215.0f);
    game->fps_camera.movement_speed = 100.0f;

    fw64_renderer_set_clear_color(engine->renderer, 39, 58, 93);
    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12);
    game->terrain = fw64_terrain_load(engine->assets, FW64_ASSET_terrain_fogworld);
}

void game_update(Game* game){
    if (fw64_input_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_L)) {
        game->fps_camera.camera.transform.position.y -= 50 * game->engine->time->time_delta;
    }
    if (fw64_input_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_R)) {
        game->fps_camera.camera.transform.position.y += 50 * game->engine->time->time_delta;
    }

    fw64_fps_camera_update(&game->fps_camera, game->engine->time->time_delta);

    Vec3* pos = &game->fps_camera.camera.transform.position;
    sprintf(game->position_text, "Pos: %.2f, %.2f, %.2f", pos->x, pos->y, pos->z);
    sprintf(game->render_text, "Meshes: %d", fw64_terrain_get_mesh_count(game->terrain));
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, &game->fps_camera.camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_terrain_draw(game->terrain, game->engine->renderer);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_NOSWAP);

    fw64_renderer_begin(game->engine->renderer, &game->fps_camera.camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_NOCLEAR);
    fw64_renderer_draw_text(game->engine->renderer, game->font, 10, 10, game->position_text);
    fw64_renderer_draw_text(game->engine->renderer, game->font, 10, 25, game->render_text);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
