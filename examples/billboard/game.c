#include "game.h"

#include "assets.h"

#include "framework64/n64/controller_button.h"

#include <math.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_fps_camera_init(&game->fps, engine->input);
    game->fps.movement_speed = 70.0f;
    game->fps.camera.near = 1.0f;
    game->fps.camera.far = 200.0f;
    fw64_camera_update_projection_matrix(&game->fps.camera);
    vec3_set(&game->fps.camera.transform.position, 0.6f, 12.0f, 20.0f);

    fw64Image* flame_image = fw64_image_load(engine->assets, FW64_ASSET_image_fire_sprite);
    flame_init(&game->flame, engine, flame_image);
    vec3_set(&game->flame.entity.transform.scale, 10.0f, 10.0f, 10.0f);
    vec3_set(&game->flame.entity.transform.position, 0, 10.0f, 0.0f);

    entity_init(&game->campfire, fw64_mesh_load(engine->assets, FW64_ASSET_mesh_campfire));
    vec3_set(&game->campfire.transform.scale, 0.5f, 0.5f, 0.5f);
    entity_refresh(&game->campfire);
}

void game_update(Game* game){
    fw64_fps_camera_update(&game->fps, game->engine->time->time_delta);

    flame_update(&game->flame, game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, &game->fps.camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);

    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->campfire.transform, game->campfire.mesh);

    flame_draw(&game->flame, game->engine->renderer);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
