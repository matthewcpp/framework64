#include "game.h"
#include "assets/assets.h"
#include "assets/scene_Fogworld.h"

#include "framework64/collision.h"
#include "framework64/math.h"
#include "framework64/util/mesh_data_itr.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/texture_util.h"

#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

#define CAMERA_EYE_OFFSET 20.0f

static void fog_settings_init(FogSettings* settings, fw64Renderer* renderer, fw64RenderPass* renderpass);
static void fog_settings_update(FogSettings* settings);

static void set_camera_ypos(Game* game);

static void ui_init(Ui* ui, fw64Engine* engine, FogSettings* settings);
static void ui_update(Ui* ui);
static void ui_update_spritebatch(Ui* ui);
static void ui_draw(Ui* ui);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->mode = GAME_MODE_PLAYING;
    game->scene = fw64_assets_load_scene(engine->assets, FW64_ASSET_scene_Fogworld, allocator);

    fw64Node* camera_node = fw64_scene_get_node(game->scene, FW64_scene_Fogworld_node_Player);
    fw64_camera_init(&game->camera, camera_node, display);
    game->camera.near = 10.0f;
    game->camera.far = 500.0f;
    fw64_camera_update_projection_matrix(&game->camera);

    fw64_fps_camera_init(&game->fps_camera, engine->input, &game->camera);
    game->fps_camera.movement_speed = 100.0f;

    game->renderpass = fw64_renderpass_create(display, allocator);
    fog_settings_init(&game->fog_settings, engine->renderer, game->renderpass);
    ui_init(&game->ui, engine, &game->fog_settings);
}

void game_update(Game* game){
    if (game->mode == GAME_MODE_PLAYING) {
        if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
            game->mode  = GAME_MODE_PAUSED;
            game->ui.active = 1;
        } else {
            fw64_fps_camera_update(&game->fps_camera, game->engine->time->time_delta);
            set_camera_ypos(game);
        }
    } 
    else {
        ui_update(&game->ui);
        game->mode = game->ui.active ? GAME_MODE_PAUSED : GAME_MODE_PLAYING;
    }
}

void set_camera_ypos(Game* game) {
    Vec3 origin = game->fps_camera.camera->node->transform.position;
    origin.y = 1000.0f;
    Vec3 dir = { 0.0f, -1.0f, 0.0f };

    fw64RaycastHit hit;
    if (!fw64_scene_raycast(game->scene, &origin, &dir, 0xFFFFFFFF, &hit))
        return;

    Vec3 a, b, c, point;
    float dist;
    fw64MeshDataItr itr;

    fw64Mesh* mesh = hit.node->mesh_instance->mesh;
    uint32_t primitive_count = fw64_mesh_get_primitive_count(mesh);
    for (uint32_t i = 0; i < primitive_count; i++) {
        fw64_mesh_data_itr_init(&itr, mesh, i);

        while(fw64_mesh_data_itr_next(&itr)) {
            fw64_mesh_data_itr_get_triangle_points(&itr, &a, &b, &c);

            if (fw64_collision_test_ray_triangle(&origin, &dir, &a, &b, &c, &point, &dist)) {
                point.y += CAMERA_EYE_OFFSET;
                game->fps_camera.camera->node->transform.position = point;
                return;
            }
        }
    }
}

void game_draw(Game* game) {
    fw64Frustum view_frustum;
    fw64_camera_extract_frustum_planes(game->fps_camera.camera, &view_frustum);

    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_set_camera(game->renderpass, game->fps_camera.camera);
    fw64_scene_draw_frustrum(game->scene, game->renderpass, &view_frustum, ~0U);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);

    if (game->mode == GAME_MODE_PAUSED) {
        ui_draw(&game->ui);
    }
}

void ui_init(Ui* ui, fw64Engine* engine, FogSettings* settings) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    ui->engine = engine;
    ui->fog_settings = settings;
    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui->overlay = fw64_texture_util_create_from_loaded_image(engine->assets, FW64_ASSET_image_overlay, allocator);
    ui->active = 0;

    ui->spritebatch = fw64_spritebatch_create(2, allocator);
    ui->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(ui->renderpass);

    fw64_ui_navigation_init(&ui->nav, engine->input, 0);
    ui_update_spritebatch(ui);
}

static void ui_update_spritebatch(Ui* ui) {
    char text[64];
    int line_height = fw64_font_line_height(ui->font);

    fw64_spritebatch_begin(ui->spritebatch);

    int pos_x = 50, pos_y = 50;
    fw64_spritebatch_set_active_layer(ui->spritebatch, 0);
    fw64_spritebatch_draw_sprite_slice_rect(ui->spritebatch, ui->overlay, 0, pos_x, pos_y, 128, 64);
    pos_y += 5;

    fw64_spritebatch_set_active_layer(ui->spritebatch, 1);
    int offset = ui->active_setting * line_height;
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, "*", pos_x + 5, pos_y + offset);
    pos_x += 10;

    sprintf(text, "Fog: %s", ui->fog_settings->enabled ? "ON" : "OFF");
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, text, pos_x, pos_y);
    pos_y += line_height;

    sprintf(text, "Begin: %0.1f", ui->fog_settings->begin);
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, text, pos_x, pos_y);
    pos_y += line_height;

    sprintf(text, "End: %0.1f", ui->fog_settings->end);
    fw64_spritebatch_draw_string(ui->spritebatch, ui->font, text, pos_x, pos_y);

    fw64_spritebatch_end(ui->spritebatch);
}

static void ui_update_active_setting(Ui* ui, float direction) {
    switch(ui->active_setting) {
        case FOG_SETTING_ENABLED: 
            ui->fog_settings->enabled = ui->fog_settings->enabled ? 0 : 1;
            break;

        case FOG_SETTING_FOG_BEGIN: 
            ui->fog_settings->begin = fw64_clamp(ui->fog_settings->begin + 0.1f * direction, 0, ui->fog_settings->end - 0.1f);
            break;

        case FOG_SETTING_FOG_END: 
            ui->fog_settings->end = fw64_clamp(ui->fog_settings->end + 0.1f * direction, ui->fog_settings->begin + 0.1f, 1.0f);
            break;
    }

    fog_settings_update(ui->fog_settings);
    ui_update_spritebatch(ui);
}

void ui_update(Ui* ui) {
    fw64_ui_navigation_update(&ui->nav, ui->engine->time->time_delta);

    if (fw64_ui_navigation_back(&ui->nav) || fw64_input_controller_button_pressed(ui->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        ui->active = 0;
        ui->active_setting = 0;
        return;
    }

    if (fw64_ui_navigation_moved_right(&ui->nav)) {
        ui_update_active_setting(ui, 1.0f);
    } else if (fw64_ui_navigation_moved_left(&ui->nav)) {
        ui_update_active_setting(ui, -1.0f);
    } else if (fw64_ui_navigation_moved_up(&ui->nav)) {
        ui->active_setting -= 1;
    } else if (fw64_ui_navigation_moved_down(&ui->nav)) {
        ui->active_setting += 1;
    }

    if (ui->active_setting < 0) {
        ui->active_setting  = FOG_SETTING_COUNT - 1;
    } else if (ui->active_setting >= FOG_SETTING_COUNT) {
        ui->active_setting = 0;
    }

    if (ui->nav.did_nav) {
        ui_update_spritebatch(ui);
    }
}

void ui_draw(Ui* ui) {
    fw64_renderpass_begin(ui->renderpass);
    fw64_renderpass_draw_sprite_batch(ui->renderpass, ui->spritebatch);
    fw64_renderpass_end(ui->renderpass);

    fw64_renderer_submit_renderpass(ui->engine->renderer, ui->renderpass);
}

static void fog_settings_init(FogSettings* settings, fw64Renderer* renderer, fw64RenderPass* renderpass) {
    settings->enabled = 1;
    fw64_color_rgba8_set(&settings->color, 51, 187, 255, 255);
    settings->begin = 0.5;
    settings->end = 1.0;
    settings->renderer = renderer;
    settings->renderpass = renderpass;
    fog_settings_update(settings);
}

static void fog_settings_update(FogSettings* settings) {
    fw64_renderpass_set_fog_positions(settings->renderpass, settings->begin, settings->end);
    fw64_renderpass_set_fog_color(settings->renderpass, settings->color.r, settings->color.b, settings->color.a);
    fw64_renderpass_set_clear_color(settings->renderpass, settings->color.r, settings->color.b, settings->color.a);
    fw64_renderpass_set_fog_enabled(settings->renderpass, settings->enabled);
}
