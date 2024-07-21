#include "game.h"

#include "assets/assets.h"
#include "assets/catherine_animation.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/skinned_mesh_instance.h"

#include <stdio.h>

static void set_animation(Game* game, int animation);
static void set_animation_speed(Game* game, float delta);
static void toggle_playing_state(Game* game);
static void ui_init(Ui* ui, fw64Engine* engine, fw64SkinnedMeshInstance* skinned_mesh_instance);
static void ui_update(Ui* ui, int animation_index);
static void ui_draw(Ui* ui);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->current_animation = 0;
    game->current_speed = 1.0f;

    fw64_renderer_set_clear_color(engine->renderer, 100,149,237);

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    scene_info.node_count = 1;
    scene_info.skinned_mesh_count = 1;
    scene_info.skinned_mesh_instance_count = 1;

    fw64_scene_init(&game->scene, &scene_info, engine->assets, allocator);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64SkinnedMesh* skinned_mesh = fw64_scene_load_skinned_mesh_asset(&game->scene, FW64_ASSET_skinnedmesh_catherine);
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_create_skinned_mesh_instance(&game->scene, node, skinned_mesh, game->current_animation);
    
    fw64_arcball_init(&game->arcball, engine->input, display);

    fw64_arcball_set_initial(&game->arcball, &skinned_mesh_instance->mesh_instance.render_bounds);
    game->arcball.camera.near = 150.0f;
    game->arcball.camera.far = 750.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);

    fw64_animation_controller_play(&skinned_mesh_instance->controller);

    game->renderpass = fw64_renderpass_create(display, allocator);
    ui_init(&game->ui, engine, skinned_mesh_instance);
    ui_update(&game->ui, game->current_animation);
}

void game_update(Game* game) {
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_scene_update(&game->scene, game->engine->time->time_delta);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        set_animation(game, game->current_animation + 1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        set_animation(game, game->current_animation -1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        set_animation_speed(game, 0.1f);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        set_animation_speed(game, -0.1f);
    }

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        toggle_playing_state(game);
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(&game->arcball.camera, &frustum);

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderpass_set_camera(game->renderpass, &game->arcball.camera);
    fw64_renderpass_begin(game->renderpass);
    fw64_scene_draw_frustrum(&game->scene, game->renderpass, &frustum, ~0U);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(renderer, game->renderpass);
    ui_draw(&game->ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

static void set_animation(Game* game, int animation) {
    game->current_animation = animation;
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_get_skinned_mesh_instance(&game->scene, 0);
    fw64AnimationData* animation_data = skinned_mesh_instance->skinned_mesh->animation_data;

    if (game->current_animation >= (int)animation_data->animation_count) {
        game->current_animation = 0;
    } else if (game->current_animation < 0) {
        game->current_animation = animation_data->animation_count - 1;
    }

    fw64_animation_controller_set_animation(&skinned_mesh_instance->controller, game->current_animation);

    // note that the idle animation is sped up to reduce frame counts when exported from blender
    if (game->current_animation == catherine_animation_Idle) {
        skinned_mesh_instance->controller.speed = 0.1f;
    } else {
        skinned_mesh_instance->controller.speed = 1.0f;
    }

    ui_update(&game->ui, game->current_animation);
}

void set_animation_speed(Game* game, float delta) {
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_get_skinned_mesh_instance(&game->scene, 0);
    skinned_mesh_instance->controller.speed += delta;
    ui_update(&game->ui, game->current_animation);
}

void toggle_playing_state(Game* game) {
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_get_skinned_mesh_instance(&game->scene, 0);
    if (skinned_mesh_instance->controller.state == FW64_ANIMATION_STATE_PLAYING) {
        fw64_animation_controller_pause(&skinned_mesh_instance->controller);
    } else {
        fw64_animation_controller_play(&skinned_mesh_instance->controller);
    }
}

void ui_init(Ui* ui, fw64Engine* engine, fw64SkinnedMeshInstance* skinned_mesh_instance) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    fw64Allocator* allocator = fw64_default_allocator();

    ui->engine = engine;
    ui->skinned_mesh_instance = skinned_mesh_instance;
    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui->spritebatch = fw64_spritebatch_create(1, allocator);
    ui->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_depth_testing_enabled(ui->renderpass, 0);
}

static const char* animation_names[catherine_animation__count__] = {
    catherine_animation_Attack1_name,
    catherine_animation_Idle_name,
    catherine_animation_Run_name,
    catherine_animation_Tired_name,
    catherine_animation_Walk_name
};

void ui_update(Ui* ui, int animation_index) {
    fw64AnimationController* controller = &ui->skinned_mesh_instance->controller;
    char status_text[32];
    sprintf(status_text, "%s (%.1f)",animation_names[animation_index], controller->speed );

    fw64_spritebatch_begin(ui->spritebatch);
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, status_text, 10, 10);
    fw64_spritebatch_end(ui->spritebatch);
}

void ui_draw(Ui* ui) {
    fw64_renderpass_begin(ui->renderpass);
    fw64_renderpass_draw_sprite_batch(ui->renderpass, ui->spritebatch);
    fw64_renderpass_end(ui->renderpass);

    fw64_renderer_submit_renderpass(ui->engine->renderer, ui->renderpass);
}
