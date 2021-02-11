#include "game.h"

#include "assets.h"

#include <math.h>

void game_init(Game* example, System* system) {
    example->system = system;

    fps_camera_init(&example->fps, system->input);
    vec3_set(&example->fps.camera.transform.position, 0.0f, 4.0f, 20.0f);

    entity_init(&example->solid_cube);

    sprite_load(ASSET_sprite_nintendo_seal, &example->nintendo_seal_sprite);
    billboard_quad_init(&example->nintendo_seal_quad, BILLBOARD_QUAD_1X1, &example->nintendo_seal_sprite);
    vec3_set(&example->nintendo_seal_quad.transform.position, -3.0f, 5.0f, 0.0f);

    sprite_load(ASSET_sprite_n64_logo, &example->n64_logo_sprite);
    billboard_quad_init(&example->n64_logo_quad, BILLBOARD_QUAD_2X2, &example->n64_logo_sprite);
    vec3_set(&example->n64_logo_quad.transform.position, 3.0f, 5.0f, 0.0f);
}

void game_update(Game* game, float time_delta){
    fps_camera_update(&game->fps, time_delta);
}

void game_draw(Game* game) {
    renderer_begin(game->system->renderer, &game->fps.camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    solid_object_draw(&game->solid_cube, game->system->renderer);
    renderer_draw_billboard_quad(game->system->renderer, &game->nintendo_seal_quad);
    renderer_draw_billboard_quad(game->system->renderer, &game->n64_logo_quad);
    renderer_end(game->system->renderer, RENDERER_FLAG_SWAP);
}
