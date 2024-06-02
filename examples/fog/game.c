#include "game.h"
#include "assets/assets.h"

#include "framework64/collision.h"
#include "framework64/math.h"
#include "framework64/util/mesh_data_itr.h"

#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

#define CAMERA_EYE_OFFSET 20.0f

static void update_fog_settings(Game* game);
static void draw_fog_settings(Game* GAME_MODE_PLAYING);
static void set_camera_ypos(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;
    game->mode = GAME_MODE_PLAYING;
    fw64_fps_camera_init(&game->fps_camera, engine->input, fw64_displays_get_primary(engine->displays));

    vec3_set(&game->fps_camera.camera.transform.position, -164.0f, 45.0f, 0.0f);
    game->fps_camera.movement_speed = 100.0f;
    game->fps_camera.camera.near = 10.0f;
    game->fps_camera.camera.far = 500.0f;
    fw64_camera_update_projection_matrix(&game->fps_camera.camera);

    game->scene = fw64_assets_load_scene(engine->assets, FW64_ASSET_scene_Fogworld, allocator);
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->overlay = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_overlay, allocator), allocator);

    fw64_renderer_set_clear_color(engine->renderer, 51, 187, 255);

    game->fog_begin = 0.5;
    game->fog_end = 1.0;
    fw64_renderer_set_fog_color(engine->renderer, 51, 187, 255);
    fw64_renderer_set_fog_positions(engine->renderer, game->fog_begin, game->fog_end);
    fw64_renderer_set_fog_enabled(engine->renderer, 1);
}

void game_update(Game* game){
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        game->mode = game->mode == GAME_MODE_PLAYING ? GAME_MODE_PAUSED : GAME_MODE_PLAYING;
        game->fog_setting = FOG_SETTING_ENABLED;
    }

    if (game->mode != GAME_MODE_PLAYING) {
        update_fog_settings(game);
        return;
    }
    
    fw64_fps_camera_update(&game->fps_camera, game->engine->time->time_delta);
    set_camera_ypos(game);
}

void set_camera_ypos(Game* game) {
    Vec3 origin = game->fps_camera.camera.transform.position;
    origin.y = 1000.0f;
    Vec3 dir = { 0.0f, -1.0f, 0.0f };

    fw64RaycastHit hit;
    if (!fw64_scene_raycast(game->scene, &origin, &dir, 0xFFFFFFFF, &hit))
        return;

    Vec3 a, b, c, point;
    float dist;
    fw64MeshDataItr itr;

    int primitive_count = fw64_mesh_get_primitive_count(hit.node->mesh);
    for (int i = 0; i < primitive_count; i++) {
        
        fw64_mesh_data_itr_init(&itr, hit.node->mesh, i);

        while(fw64_mesh_data_itr_next(&itr)) {
            
            fw64_mesh_data_itr_get_triangle_points(&itr, &a, &b, &c);

            if (fw64_collision_test_ray_triangle(&origin, &dir, &a, &b, &c, &point, &dist)) {
                point.y += CAMERA_EYE_OFFSET;
                game->fps_camera.camera.transform.position = point;
                return;
            }
        }
    }
}

void game_draw(Game* game) {
    fw64Frustum view_frustum;
    fw64_camera_extract_frustum_planes(&game->fps_camera.camera, &view_frustum);

    fw64_renderer_set_anti_aliasing_enabled(game->engine->renderer, 1);
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->fps_camera.camera);

    fw64_scene_draw_frustrum(game->scene, game->engine->renderer, &view_frustum);

    if (game->mode == GAME_MODE_PAUSED) {
        fw64_renderer_set_anti_aliasing_enabled(game->engine->renderer, 0);
        draw_fog_settings(game);
    }
        
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void update_fog_settings(Game* game) {
    float direction = 0.0f;
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT) ||
        fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        direction = 1.0f;
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT) ||
        fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        direction = -1.0f;
    }
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP) ||
        fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        game->fog_setting = game->fog_setting == FOG_SETTING_ENABLED ? FOG_SETTING_FOG_END : game->fog_setting - 1;
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN) ||
        fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        game->fog_setting = game->fog_setting == FOG_SETTING_FOG_END ? FOG_SETTING_ENABLED : game->fog_setting + 1;
    }

    if (direction == 0.0f)
        return;

    switch(game->fog_setting) {
        case FOG_SETTING_ENABLED: 
            fw64_renderer_set_fog_enabled(game->engine->renderer, !fw64_renderer_get_fog_enabled(game->engine->renderer));
            break;

        case FOG_SETTING_FOG_BEGIN: 
            game->fog_begin = fw64_clamp(game->fog_begin + (0.1 * direction), 0.0f, game->fog_end);
            fw64_renderer_set_fog_positions(game->engine->renderer, game->fog_begin, game->fog_end);
            break;

        case FOG_SETTING_FOG_END: 
            game->fog_end = fw64_clamp(game->fog_end + (0.1 * direction), game->fog_begin, 1.0f);
            fw64_renderer_set_fog_positions(game->engine->renderer, game->fog_begin, game->fog_end);
            break;
    }
}

static void draw_fog_settings(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    char text[64];

    int pos_x = 50, pos_y = 50;
    fw64_renderer_draw_sprite_slice_transform(renderer, game->overlay, 0, pos_x - 20, pos_y - 5, 4.0f, 2.0f, 0.0f);

    if (game->fog_setting == FOG_SETTING_ENABLED)
        fw64_renderer_draw_text(renderer, game->font, pos_x - 20, pos_y, "*");

    sprintf(text, "Fog: %s", fw64_renderer_get_fog_enabled(renderer) ? "ON" : "OFF");
    fw64_renderer_draw_text(renderer, game->font, pos_x, pos_y, text);
    pos_y += 16;

    if (game->fog_setting == FOG_SETTING_FOG_BEGIN)
        fw64_renderer_draw_text(renderer, game->font, pos_x - 20, pos_y, "*");

    sprintf(text, "Begin: %0.1f", game->fog_begin);
    fw64_renderer_draw_text(renderer, game->font, pos_x, pos_y, text);
    pos_y += 16;

    if (game->fog_setting == FOG_SETTING_FOG_END)
        fw64_renderer_draw_text(renderer, game->font, pos_x - 20, pos_y, "*");
        
    sprintf(text, "End: %0.1f", game->fog_end);
    fw64_renderer_draw_text(renderer, game->font, pos_x, pos_y, text);
}