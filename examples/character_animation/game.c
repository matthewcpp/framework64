#include "game.h"

#include "assets.h"
#include "catherine_animation.h"

#include "framework64/n64/controller_button.h"

static void set_animation(Game* game, int animation);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_renderer_set_clear_color(engine->renderer, 100,149,237);

    game->current_animation = 0;
    game->current_speed = 1.0f;

    fw64_arcball_init(&game->arcball, engine->input);
    fw64_node_init(&game->node);
    game->mesh = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_catherine, NULL);

    Box bounding_box;
    fw64_mesh_get_bounding_box(game->mesh, &bounding_box);
    fw64_arcball_set_initial(&game->arcball, &bounding_box);
    game->arcball.camera.near = 150.0f;
    game->arcball.camera.far = 750.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);

    game->animation_data = fw64_animation_data_load(engine->assets, FW64_ASSET_animation_data_catherine, NULL);
    fw64_animation_controller_init(&game->animation_state, game->animation_data, game->current_animation, NULL);
    fw64_animation_controller_play(&game->animation_state);

    //game->consolas = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12, NULL);
    //game->button_sprite = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_buttons, NULL), NULL);
}


void game_update(Game* game) {
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_animation_controller_update(&game->animation_state, game->engine->time->time_delta);

    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        set_animation(game, game->current_animation + 1);
    }
    else if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        set_animation(game, game->current_animation -1);
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->arcball.camera);
    fw64_renderer_draw_animated_mesh(renderer, game->mesh, &game->animation_state, &game->node.transform);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

static void set_animation(Game* game, int animation) {
    game->current_animation = animation;

    if (game->current_animation >= game->animation_data->animation_count)
        game->current_animation = 0;
    else if (game->current_animation < 0)
        game->current_animation = game->animation_data->animation_count - 1;

    fw64_animation_controller_set_animation(&game->animation_state, game->current_animation);

    // note that the idle animation is sped up to reduce frame counts when exported from blender
    if (game->current_animation == catherine_animation_Idle)
        game->animation_state.speed = 0.1f;
    else
        game->animation_state.speed = 1.0f;
}