#include "game.h"
#include "assets/assets.h"

#include <framework64/math.h>
#include "framework64/matrix.h"
#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

static void setup_initial_viewport(Game* game);
static void setup_scene(Game* game);
static void setup_renderpasses(Game* game);
static void update_viewport_info(Game* game);


void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();

    game->engine = engine;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->camera_pos_delta = 4;
    game->camera_size_delta = 4;
    game->spritebatch = fw64_spritebatch_create(1, allocator);

    setup_scene(game);
    setup_initial_viewport(game);
    setup_renderpasses(game);
}

void setup_renderpasses(Game* game) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(game->engine->displays);

    game->renderpasses[RENDER_PASS_VIEW] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_clear_flags(game->renderpasses[RENDER_PASS_VIEW] , FW64_CLEAR_FLAG_COLOR);
    fw64_renderpass_set_clear_color(game->renderpasses[RENDER_PASS_VIEW] , 55, 55, 55);

    game->renderpasses[RENDER_PASS_INFO] = fw64_renderpass_create(display, allocator);
    Vec2 display_size = fw64_display_get_size_f(display);
    float projection[16];
    matrix_ortho2d(projection, 0, display_size.x, display_size.y, 0);
    fw64_renderpass_set_projection_matrix(game->renderpasses[RENDER_PASS_INFO], projection, NULL);
    fw64_renderpass_set_depth_testing_enabled(game->renderpasses[RENDER_PASS_INFO], 0);
}

void setup_initial_viewport(Game* game) {
    fw64Display* primary_display = fw64_displays_get_primary(game->engine->displays);
    IVec2 display_size = fw64_display_get_size(primary_display);
    IVec2 viewport_size = {display_size.x / 2, display_size.y / 2};

    IVec2 viewport_pos = {(display_size.x - viewport_size.x) / 2, (display_size.y - viewport_size.y) / 2};
    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    vec3_set(&camera_node->transform.position, 0.0f, 7.5f, 18.0f);
    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = vec3_up();
    fw64_transform_look_at(&camera_node->transform, &target, &up);

    fw64_camera_init(&game->camera, camera_node, primary_display);
    fw64_camera_set_viewport(&game->camera, &viewport_pos, &viewport_size);
}

void setup_scene(Game* game) {
    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 2;
    info.mesh_count = 1;
    info.mesh_instance_count = 1;

    fw64_scene_init(&game->scene, &info, game->engine->assets, fw64_default_allocator());
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_n64_logo);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    vec3_set_all(&node->transform.scale, 0.1f);
    fw64_node_update(node);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);

    fw64_rotate_node_init(&game->rotate_node, node);
}

void game_update(Game* game){
    IVec2 updated_pos = game->camera.viewport.position;
    IVec2 updated_size = game->camera.viewport.size;

    fw64_rotate_node_update(&game->rotate_node, game->engine->time->time_delta);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        setup_initial_viewport(game);
        return;
    }

    if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        updated_pos.y -= game->camera_pos_delta;
    } else if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        updated_pos.y += game->camera_pos_delta;
    } else if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        updated_pos.x -= game->camera_pos_delta;
    } else if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        updated_pos.x += game->camera_pos_delta;
    }

    if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
        updated_size.y -= game->camera_pos_delta;
    } else if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
        updated_size.y += game->camera_pos_delta;
    } else if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        updated_size.x -= game->camera_pos_delta;
    } else if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        updated_size.x += game->camera_pos_delta;
    }

    game->camera.aspect = (float)updated_size.x / (float)updated_size.y;
    fw64_camera_update_projection_matrix(&game->camera);

    updated_pos.x = fw64_maxi(updated_pos.x, 0);
    updated_pos.y = fw64_maxi(updated_pos.y, 0);
    fw64_camera_set_viewport(&game->camera, &updated_pos, &updated_size);

    update_viewport_info(game);
}

void update_viewport_info(Game* game) {
    fw64Viewport* vp = &game->camera.viewport;
    char viewport_info_txt[32];
    int y_pos = 10;
    
    fw64_spritebatch_begin(game->spritebatch);

    sprintf(viewport_info_txt, "Pos: %d, %d", vp->position.x, vp->position.y);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, viewport_info_txt, 10, y_pos);
    y_pos += fw64_font_line_height(game->font);

    sprintf(viewport_info_txt, "Size: %d, %d", vp->size.x, vp->size.y);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, viewport_info_txt, 10, y_pos);

    fw64_spritebatch_end(game->spritebatch);
}

void game_draw(Game* game) {
    fw64RenderPass* pass = game->renderpasses[RENDER_PASS_VIEW];
    fw64_renderpass_set_camera(pass, &game->camera);
    fw64_renderpass_begin(pass);
    fw64_scene_draw_all(&game->scene, pass);
    fw64_renderpass_end(pass);
    fw64_renderer_submit_renderpass(game->engine->renderer, pass);

    pass = game->renderpasses[RENDER_PASS_INFO];
    fw64_renderpass_begin(pass);
    fw64_renderpass_draw_sprite_batch(pass, game->spritebatch);
    fw64_renderpass_end(pass);
    fw64_renderer_submit_renderpass(game->engine->renderer, pass);
}
