#include "game.h"
#include "assets.h"

#include "framework64/n64/controller_button.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_fps_camera_init(&game->fps_camera, engine->input);
    game->fps_camera.camera.transform.position.y = -50;
    game->fps_camera.movement_speed = 75.0f;

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
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, &game->fps_camera.camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_terrain_draw(game->terrain, game->engine->renderer);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
