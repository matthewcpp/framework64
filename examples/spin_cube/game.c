#include "game.h"
#include "assets.h"

#include "framework64/n64/controller_button.h"

#define ROTATION_SPEED 90.0f

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_camera_init(&game->camera);
    vec3_set(&game->camera.transform.position, 0.0f, 0.0f, 5.0f);
    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    fw64_transform_look_at(&game->camera.transform, &target, &up);
    fw64_camera_update_view_matrix(&game->camera);

    game->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED;

    fw64_node_init(&game->solid_cube);
    fw64_node_set_mesh(&game->solid_cube, fw64_mesh_load(engine->assets, FW64_ASSET_mesh_blue_cube, NULL));
    fw64_node_init(&game->wire_cube);
    fw64_node_set_mesh(&game->wire_cube, fw64_mesh_load(engine->assets, FW64_ASSET_mesh_blue_cube_wire, NULL));

    game->rotation = 0.0f;
}

void game_update(Game* game){
    game->rotation += game->engine->time->time_delta * ROTATION_SPEED;

    quat_from_euler(&game->solid_cube.transform.rotation, 0, game->rotation, 0.0f);
    fw64_node_update(&game->solid_cube);

    quat_from_euler(&game->wire_cube.transform.rotation, 0, game->rotation, 0.0f);
    fw64_node_update(&game->wire_cube);

    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT))
        game->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME;
    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP))
        game->draw_mode = EXAMPLE_DRAW_MODE_SHADED;
    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT))
        game->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED;
}

static void line_example_draw_wireframe(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_RENDERER_MODE_LINES,  FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->camera);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->wire_cube.transform, game->wire_cube.mesh);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

static void line_example_draw_solid(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->camera);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->solid_cube.transform, game->solid_cube.mesh);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

static void line_example_draw_wireframe_on_shaded(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);
    fw64_renderer_draw_static_mesh(renderer, &game->solid_cube.transform, game->solid_cube.mesh);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_NOSWAP);

    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_LINES, FW64_RENDERER_FLAG_NOCLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);
    fw64_renderer_draw_static_mesh(renderer, &game->wire_cube.transform, game->wire_cube.mesh);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void game_draw(Game* game) {
    switch (game->draw_mode)
    {
    case EXAMPLE_DRAW_MODE_WIREFRAME:
        line_example_draw_wireframe(game);
        break;

    case EXAMPLE_DRAW_MODE_SHADED:
        line_example_draw_solid(game);
        break;

    case EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED:
        line_example_draw_wireframe_on_shaded(game);
        break;
    
    default:
        break;
    }
}
