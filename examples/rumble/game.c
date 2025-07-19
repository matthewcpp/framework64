#include "game.h"
#include "assets/assets.h"

#include "framework64/math.h"
#include "framework64/random.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/quad.h"

#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

static void shake_quad(Game* game);
static void set_shake_speed(Game* game);
static void update_spritebatch(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->rumble_frequency = 0.5f;
    game->rumble_duration = 2.0f;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->buttons = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_buttons, allocator), allocator);
    game->spritebatch = fw64_spritebatch_create(1, allocator);

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 2;
    info.mesh_instance_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64Mesh* mesh = fw64_textured_quad_create(game->engine, FW64_ASSET_image_n64_logo, allocator);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);

    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    vec3_set(&camera_node->transform.position, 0.0f, 0.0f, 5.0f);
    fw64_node_update(camera_node);
    fw64Camera camera;
    fw64_camera_init(&camera, camera_node, display);
    fw64_renderpass_set_camera(game->renderpasses[RENDER_PASS_SCENE], &camera);

    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

    set_shake_speed(game);
    game->shake_time = 0.0f;
}

void game_update(Game* game){
    int controller = 0;
    fw64Input* input = game->engine->input;

    if (fw64_input_controller_button_pressed(input, controller, FW64_N64_CONTROLLER_BUTTON_A)) {
        fw64_input_controller_set_rumble(input, 0, game->rumble_frequency, game->rumble_duration);
    }

    if (fw64_input_controller_button_pressed(input, controller, FW64_N64_CONTROLLER_BUTTON_B)) {
        fw64_input_controller_set_rumble(input, 0, 0.0f, 0.0f);
    }

    int controller_is_rumbling = fw64_input_controller_rumble_active(input, controller);

    if (fw64_input_controller_button_pressed(input, controller, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        game->rumble_frequency = fw64_minf(game->rumble_frequency + 0.1f, 1.0f);
        set_shake_speed(game);
        
        if (controller_is_rumbling)
            fw64_input_controller_set_rumble(input, 0, game->rumble_frequency, game->rumble_duration);
    }

    if (fw64_input_controller_button_pressed(input, controller, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        game->rumble_frequency = fw64_maxf(game->rumble_frequency - 0.1f, 0.0f);
        set_shake_speed(game);

        if (controller_is_rumbling)
            fw64_input_controller_set_rumble(input, 0, game->rumble_frequency, game->rumble_duration);
    }

    if (controller_is_rumbling) {
        shake_quad(game);
    }
    else {
        fw64Node* node = fw64_scene_get_node(&game->scene, 0);
        vec3_set_zero(&node->transform.position);
        fw64_node_update(node);
    }
}

void game_draw(Game* game) {
    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_begin(renderpass);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, game->spritebatch);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);
}

void shake_quad(Game* game) {
    game->shake_time -= game->engine->time->time_delta;

    if (game->shake_time <= 0.0f) {
        fw64Node* node = fw64_scene_get_node(&game->scene, 0);
        vec3_set(&node->transform.position, fw64_random_float_in_range(-0.5, 0.5), fw64_random_float_in_range(-0.5, 0.5), 0.0f);
        fw64_node_update(node);

        game->shake_time = game->shake_speed;
    }
}

static void set_shake_speed(Game* game) {
    game->shake_speed = (1.1f - game->rumble_frequency) / 2.0f;
    update_spritebatch(game);
}

void update_spritebatch(Game* game) {
    char text[32];
    IVec2 viewport_size = fw64_display_get_size(fw64_displays_get_primary(game->engine->displays));
    int draw_x = 20, draw_y = 20;

    fw64_spritebatch_begin(game->spritebatch);
    
    fw64_spritebatch_draw_sprite_slice(game->spritebatch, game->buttons, 0, draw_x, draw_y);
    draw_x += fw64_texture_slice_width(game->buttons) + 5;
    fw64_spritebatch_draw_string(game->spritebatch, game->font, "Start", draw_x, draw_y);
    draw_x += 40;

    fw64_spritebatch_draw_sprite_slice(game->spritebatch, game->buttons, 1, draw_x, draw_y);
    draw_x += fw64_texture_slice_width(game->buttons) + 5;
    fw64_spritebatch_draw_string(game->spritebatch, game->font, "Stop", draw_x, draw_y);
    draw_x = 20;
    draw_y = viewport_size.y - 40;

    fw64_spritebatch_draw_sprite_slice(game->spritebatch, game->buttons, 10, draw_x, draw_y);
    draw_x += fw64_texture_slice_width(game->buttons) + 5;
    fw64_spritebatch_draw_sprite_slice(game->spritebatch, game->buttons, 11, draw_x, draw_y);
    draw_x += fw64_texture_slice_width(game->buttons) + 5;
    sprintf(text, "frequency: %.2f", game->rumble_frequency);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, text, draw_x, draw_y);
    fw64_spritebatch_end(game->spritebatch);
}
