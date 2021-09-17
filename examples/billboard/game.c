#include "game.h"

#include "assets.h"

#include "framework64/n64/controller_button.h"

#include <math.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fps_camera_init(&game->fps, engine->input);
    vec3_set(&game->fps.camera.transform.position, 0.0f, 4.0f, 20.0f);

    fw64Mesh* nintendo_seal_mesh = textured_quad_create(game->engine, fw64_assets_get_image(engine->assets, FW64_ASSET_sprite_nintendo_seal));
    entity_init(&game->nintendo_seal, nintendo_seal_mesh);
    vec3_set(&game->nintendo_seal.transform.position, -3.0f, 5.0f, 0.0f);

    fw64Mesh* n64_logo_mesh = textured_quad_create(game->engine, fw64_assets_get_image(engine->assets, FW64_ASSET_sprite_n64_logo));
    entity_init(&game->n64_logo, n64_logo_mesh);
    vec3_set(&game->n64_logo.transform.position, 3.0f, 5.0f, 0.0f);


    entity_init(&game->blue_cube, fw64_assets_get_mesh(engine->assets, FW64_ASSET_mesh_blue_cube));
}

void game_update(Game* game){
    fps_camera_update(&game->fps, game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, &game->fps.camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->blue_cube.transform, game->blue_cube.mesh);
    
    entity_billboard(&game->nintendo_seal, &game->fps.camera);
    entity_refresh(&game->nintendo_seal);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->nintendo_seal.transform, game->nintendo_seal.mesh);

    entity_billboard(&game->n64_logo, &game->fps.camera);
    entity_refresh(&game->n64_logo);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->n64_logo.transform, game->n64_logo.mesh);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
