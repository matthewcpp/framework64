#include "game.h"

#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

#include <math.h>

static void create_ground_plane(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;

    fw64_renderer_set_clear_color(engine->renderer, 0, 17, 51);

    fw64_fps_camera_init(&game->fps, engine->input, fw64_displays_get_primary(engine->displays));
    game->fps.movement_speed = 70.0f;
    game->fps.camera.near = 1.0f;
    game->fps.camera.far = 300.0f;
    fw64_camera_update_projection_matrix(&game->fps.camera);
    vec3_set(&game->fps.camera.transform.position, 0.6f, 12.0f, 40.0f);


    flame_init(&game->flame, engine);
    vec3_set(&game->flame.entity.transform.scale, 10.0f, 12.0f, 10.0f);
    vec3_set(&game->flame.entity.transform.position, 0, 11.0f, 0.0f);
    fw64_node_update(&game->flame.entity);

    fw64_node_init(&game->campfire);
    fw64_node_set_mesh(&game->campfire, fw64_assets_load_mesh(engine->assets, FW64_ASSET_mesh_campfire, allocator));
    vec3_set(&game->campfire.transform.scale, 0.5f, 0.5f, 0.5f);
    fw64_node_update(&game->campfire);

    create_ground_plane(game);

    fw64_node_init(&game->moon);
    fw64_node_set_mesh(&game->moon, fw64_textured_quad_create(engine, FW64_ASSET_image_moon, allocator));
    vec3_set(&game->moon.transform.scale, 5.0f, 5.0f, 5.0f);
    vec3_set(&game->moon.transform.position, -100.0f, 50.0f, -100.0f);
    fw64_node_update(&game->moon);
}

void create_ground_plane(Game* game) {
    fw64TexturedQuadParams params;
    fw64_textured_quad_params_init(&params);
    params.image_asset_id = FW64_ASSET_image_grass;
    params.max_s = 4.0f;
    params.max_t = 4.0f;

    fw64_node_init(&game->ground);
    fw64_node_set_mesh(&game->ground, fw64_textured_quad_create_with_params(game->engine, &params, fw64_default_allocator()));
    fw64Texture* texture = fw64_material_get_texture(fw64_mesh_get_material_for_primitive(game->ground.mesh, 0));
    fw64_texture_set_wrap_mode(texture, FW64_TEXTURE_WRAP_REPEAT, FW64_TEXTURE_WRAP_REPEAT);
    quat_from_euler(&game->ground.transform.rotation, 90.0f, 0.0f, 0.0f);
    vec3_set(&game->ground.transform.scale, 100.0f, 100.0f, 100.0f);
    fw64_node_update(&game->ground);
}

void game_update(Game* game){
    fw64_fps_camera_update(&game->fps, game->engine->time->time_delta);

    flame_update(&game->flame, game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->fps.camera);

    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->campfire.transform, game->campfire.mesh);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->ground.transform, game->ground.mesh);

    fw64_node_billboard(&game->moon, fw64_renderer_get_camera(game->engine->renderer));
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->moon.transform, game->moon.mesh);

    flame_draw(&game->flame, game->engine->renderer);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
