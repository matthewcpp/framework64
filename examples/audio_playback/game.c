#include "game.h"
#include "assets/assets.h"
#include "assets/music_bank_musicbank.h"

#include "framework64/util/quad.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/texture_util.h"
#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

static void scene_view_init(SceneView* scene_view, fw64Engine* engine);
static void scene_view_draw(SceneView* scene_view);

static void ui_init(UI* ui, fw64Engine* engine);
static void ui_draw(UI* ui);


void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    scene_view_init(&game->scene_view, engine);
    ui_init(&game->ui, engine);

    fw64_audio_load_musicbank_asset(engine->audio, engine->assets, FW64_ASSET_musicbank_musicbank);
    fw64_audio_play_music(engine->audio, music_bank_musicbank_wakening);
}

void game_update(Game* game){
    fw64AudioStatus music_status = fw64_audio_get_music_status(game->engine->audio);
    game->scene_view.rotate_node.enabled = music_status == FW64_AUDIO_PLAYING;
    fw64_rotate_node_update(&game->scene_view.rotate_node, game->engine->time->time_delta);
    float playback_speed = fw64_audio_get_playback_speed(game->engine->audio);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        fw64_audio_set_music_playback_speed(game->engine->audio, playback_speed + 0.1f);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        fw64_audio_set_music_playback_speed(game->engine->audio, playback_speed - 0.1f);
    }

    if (music_status == FW64_AUDIO_STOPPED) {
        fw64_audio_play_music(game->engine->audio, music_bank_musicbank_wakening);
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    scene_view_draw(&game->scene_view);
    ui_draw(&game->ui);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

static void scene_view_init(SceneView* scene_view, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    scene_view->engine = engine;

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 1;
    info.mesh_instance_count = 1;

    fw64_scene_init(&scene_view->scene, &info, engine->assets, allocator);

    fw64Node* node = fw64_scene_create_node(&scene_view->scene);
    fw64Mesh* mesh = fw64_textured_quad_create(engine, FW64_ASSET_image_n64_logo, allocator);
    fw64_scene_create_mesh_instance(&scene_view->scene, node, mesh);

    fw64_rotate_node_init(&scene_view->rotate_node, node);
    vec3_set(&scene_view->rotate_node.axis, 0.0f, 0.0f, 1.0f);

    fw64_camera_init(&scene_view->camera, display);

    scene_view->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_camera(scene_view->renderpass, &scene_view->camera);
}

void scene_view_draw(SceneView* scene_view) {
    fw64_renderpass_begin(scene_view->renderpass);
    fw64_scene_draw_all(&scene_view->scene, scene_view->renderpass);
    fw64_renderpass_end(scene_view->renderpass);

    fw64_renderer_submit_renderpass(scene_view->engine->renderer, scene_view->renderpass);
}

static void ui_init(UI* ui, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display =  fw64_displays_get_primary(engine->displays);
    ui->engine = engine;
    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui->sprite_batch = fw64_spritebatch_create(1, allocator);

    ui->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(ui->renderpass);
}

static void ui_draw(UI* ui) {
    char text[32];
    sprintf(text, "Playback Speed: %.1f", fw64_audio_get_playback_speed(ui->engine->audio));

    fw64_spritebatch_begin(ui->sprite_batch);
    fw64_spritebatch_draw_string(ui->sprite_batch, ui->font, text, 30, 200);
    fw64_spritebatch_end(ui->sprite_batch);

    fw64_renderpass_begin(ui->renderpass);
    fw64_renderpass_draw_sprite_batch(ui->renderpass, ui->sprite_batch);
    fw64_renderpass_end(ui->renderpass);

    fw64_renderer_submit_renderpass(ui->engine->renderer, ui->renderpass);
}
