#include "game.h"

#define ROTATION_SPEED 90.0f

void game_init(Game* game, System* system) {
    game->system = system;

    camera_init(&game->camera);
    vec3_set(&game->camera.transform.position, 0.0f, 0.0f, 5.0f);
    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    transform_look_at(&game->camera.transform, &target, &up);
    camera_update_view_matrix(&game->camera);

    game->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED;

    entity_init(&game->solid_cube);
    entity_init(&game->wire_cube);

    Color clear_color = {255,255,255};
    renderer_set_clear_color(system->renderer, &clear_color);

    game->rotation = 0.0f;
}

void game_update(Game* game, float time_delta){
    game->rotation += time_delta * ROTATION_SPEED;

    quat_from_euler(&game->solid_cube.transform.rotation, 0, game->rotation, 0.0f);
    quat_from_euler(&game->wire_cube.transform.rotation, 0, game->rotation, 0.0f);

    if (input_button_pressed(game->system->input, 0, L_CBUTTONS))
        game->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME;
    if (input_button_pressed(game->system->input, 0, U_CBUTTONS))
        game->draw_mode = EXAMPLE_DRAW_MODE_SHADED;
    if (input_button_pressed(game->system->input, 0, R_CBUTTONS))
        game->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED;
}

static void line_example_draw_wireframe(Game* game) {
    renderer_begin(game->system->renderer, &game->camera, RENDERER_MODE_LINES,  RENDERER_FLAG_CLEAR);
    wire_object_draw(&game->solid_cube, game->system->renderer);
    renderer_end(game->system->renderer, RENDERER_FLAG_SWAP);
}

static void line_example_draw_solid(Game* game) {
    renderer_begin(game->system->renderer, &game->camera, RENDERER_MODE_TRIANGLES,  RENDERER_FLAG_CLEAR);
    solid_object_draw(&game->wire_cube, game->system->renderer);
    renderer_end(game->system->renderer, RENDERER_FLAG_SWAP);
}

static void line_example_draw_wireframe_on_shaded(Game* game) {
    renderer_begin(game->system->renderer, &game->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    solid_object_draw(&game->solid_cube, game->system->renderer);
    renderer_end(game->system->renderer, RENDERER_FLAG_NOSWAP);

    renderer_begin(game->system->renderer, &game->camera, RENDERER_MODE_LINES, RENDERER_FLAG_NOCLEAR);
    wire_object_draw(&game->wire_cube, game->system->renderer);
    renderer_end(game->system->renderer, RENDERER_FLAG_SWAP);
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
