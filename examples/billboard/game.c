#include "game.h"

#include "assets.h"

#include "framework64/n64/controller_button.h"

#include <math.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_renderer_set_clear_color(engine->renderer, 0, 17, 51);

    fw64_fps_camera_init(&game->fps, engine->input);
    game->fps.movement_speed = 70.0f;
    game->fps.camera.near = 1.0f;
    game->fps.camera.far = 300.0f;
    fw64_camera_update_projection_matrix(&game->fps.camera);
    vec3_set(&game->fps.camera.transform.position, 0.6f, 12.0f, 40.0f);

    fw64Image* flame_image = fw64_image_load(engine->assets, FW64_ASSET_image_fire_sprite);
    flame_init(&game->flame, engine, flame_image);
    vec3_set(&game->flame.entity.transform.scale, 10.0f, 12.0f, 10.0f);
    vec3_set(&game->flame.entity.transform.position, 0, 11.0f, 0.0f);

    entity_init(&game->campfire, fw64_mesh_load(engine->assets, FW64_ASSET_mesh_campfire));
    vec3_set(&game->campfire.transform.scale, 0.5f, 0.5f, 0.5f);
    entity_refresh(&game->campfire);

    entity_init(&game->ground, textured_quad_create_with_params(engine, FW64_ASSET_image_grass, 4.0, 4.0));
    fw64Texture* texture = fw64_material_get_texture(fw64_mesh_get_material_for_primitive(game->ground.mesh, 0));
    fw64_texture_set_wrap_mode(texture, FW64_TEXTURE_WRAP_REPEAT, FW64_TEXTURE_WRAP_REPEAT);
    quat_from_euler(&game->ground.transform.rotation, 90.0f, 0.0f, 0.0f);
    vec3_set(&game->ground.transform.scale, 100.0f, 100.0f, 100.0f);
    entity_refresh(&game->ground);

    entity_init(&game->moon, textured_quad_create(engine, FW64_ASSET_image_moon));
    vec3_set(&game->moon.transform.scale, 5.0f, 5.0f, 5.0f);
    vec3_set(&game->moon.transform.position, -100.0f, 50.0f, -100.0f);
    entity_refresh(&game->moon);

    fw64MusicBank* music_bank = fw64_music_bank_load(engine->assets, FW64_ASSET_musicbank_musicbank2);
    fw64_audio_set_music_bank(engine->audio, music_bank);
    fw64_audio_play_music(engine->audio, 2);
}

void game_update(Game* game){
    fw64_fps_camera_update(&game->fps, game->engine->time->time_delta);

    flame_update(&game->flame, game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, &game->fps.camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);

    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->campfire.transform, game->campfire.mesh);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->ground.transform, game->ground.mesh);

    entity_billboard(&game->moon, fw64_renderer_get_camera(game->engine->renderer));
    entity_refresh(&game->moon);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->moon.transform, game->moon.mesh);

    flame_draw(&game->flame, game->engine->renderer);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
