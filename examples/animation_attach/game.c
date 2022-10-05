#include "game.h"

#include "assets/assets.h"
#include "assets/skinned_mesh_figure.h"

#include "framework64/n64/controller_button.h"

static void setup_camera(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_renderer_set_clear_color(engine->renderer, 100,149,237);

    fw64_arcball_init(&game->arcball, engine->input);
    fw64_node_init(&game->character_node);
    fw64_node_set_mesh(&game->character_node, fw64_mesh_load(engine->assets, FW64_ASSET_mesh_figure, NULL));
    vec3_set_all(&game->character_node.transform.scale, 0.1f);
    fw64_node_update(&game->character_node);
    
    game->animation_data = fw64_animation_data_load(engine->assets, FW64_ASSET_animation_data_figure, NULL);
    fw64_animation_controller_init(&game->animation_state, game->animation_data, skinned_mesh_figure_animation_Attack, NULL);
    game->animation_state.speed = 1.25f;
    fw64_animation_controller_play(&game->animation_state);

    game->sword_mesh = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_sword, NULL);
    game->mace_mesh = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_mace, NULL);
    game->current_weapon_mesh = game->sword_mesh;
    
    fw64_animated_mesh_attachment_init(&game->weapon_attachment, &game->character_node.transform, &game->animation_state, skinned_mesh_figure_joint_Hand_R);
    quat_from_euler(&game->weapon_attachment.local_rotation, 90.0f, 0.0f, 0.0f);
    vec3_set(&game->weapon_attachment.local_position, 0.0f, 13.0f, 0.0f);

    setup_camera(game);
}

void game_update(Game* game) {
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_animation_controller_update(&game->animation_state, game->engine->time->time_delta);
    fw64_animated_mesh_attachment_update(&game->weapon_attachment);


    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        if (game->animation_state.state == FW64_ANIMATION_STATE_PLAYING)
            fw64_animation_controller_pause(&game->animation_state);
        else
            fw64_animation_controller_play(&game->animation_state);
    }

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_B)) {
        fw64_animation_controller_stop(&game->animation_state);
    }

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT) || 
        fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        game->current_weapon_mesh = game->current_weapon_mesh == game->sword_mesh ? game->mace_mesh : game->sword_mesh;
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->arcball.camera);
    fw64_renderer_draw_animated_mesh(renderer, game->character_node.mesh, &game->animation_state, &game->character_node.transform);
    fw64_renderer_draw_static_mesh_matrix(renderer, &game->weapon_attachment.matrix, game->current_weapon_mesh);

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void setup_camera(Game* game) {
    Box bounding_box, transformed_box;
    float matrix[16];
    fw64Transform* transform = &game->character_node.transform;
    matrix_from_trs(&matrix[0], &transform->position, &transform->rotation, &transform->scale);

    fw64_mesh_get_bounding_box(game->character_node.mesh, &bounding_box);
    matrix_transform_box(&matrix[0], &bounding_box, &transformed_box);

    fw64_arcball_set_initial(&game->arcball, &transformed_box);
    game->arcball.camera.near = 1.0f;
    game->arcball.camera.far = 100.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);
}