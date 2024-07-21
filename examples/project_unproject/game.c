#include "game.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/texture_util.h"
#include "framework64/util/renderpass_util.h"

#include "assets/assets.h"

#include <limits.h>

void game_init(Game* game, fw64Engine* engine) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    fw64Allocator* allocator = fw64_default_allocator();

    game->engine = engine;
    game->flags = FLAG_NONE;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->crosshair = fw64_texture_util_create_from_loaded_image(engine->assets, FW64_ASSET_image_crosshair, allocator);
    fw64_arcball_init(&game->arcball_camera, engine->input, fw64_displays_get_primary(engine->displays));
    
    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 1;
    info.mesh_count = 1;
    info.mesh_instance_count = 1;
    info.collider_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_blue_cube);
    Box mesh_bounding = fw64_mesh_get_bounding_box(mesh);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);
    fw64_scene_create_box_collider(&game->scene, node, &mesh_bounding);
    fw64_arcball_set_initial(&game->arcball_camera, &mesh_bounding);

    game->crosshair_pos = fw64_display_get_size(display);
    game->crosshair_pos.x /= 2;
    game->crosshair_pos.y /= 2;

    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

    game->spritebatch = fw64_spritebatch_create(1, allocator);
}

static void move_crosshair(Game* game);

void game_update(Game* game){
    move_crosshair(game);

    if (fw64_input_controller_button_pressed(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_A)) {
        Vec3 ray_origin, ray_direction;
        
        fw64_camera_ray_from_viewport_pos(&game->arcball_camera.camera, &game->crosshair_pos, &ray_origin, &ray_direction);
        if (fw64_scene_raycast(&game->scene, &ray_origin, &ray_direction, UINT32_MAX, &game->raycast)) {
            game->flags |= FLAG_HIT_POS_ACTIVE;
        } else {
            game->flags &= ~FLAG_HIT_POS_ACTIVE;
        }
    }

    fw64_arcball_update(&game->arcball_camera, game->engine->time->time_delta);

    Vec3 min = {-1.0f, -1.0f, -1.0f}, max = {1.0f, 1.0f, 1.0f};
    fw64_camera_world_to_viewport_pos(&game->arcball_camera.camera, &min, &game->min_pos);
    fw64_camera_world_to_viewport_pos(&game->arcball_camera.camera, &max, &game->max_pos);

    if (game->flags & FLAG_HIT_POS_ACTIVE) {
        fw64_camera_world_to_viewport_pos(&game->arcball_camera.camera, &game->raycast.point, &game->hit_pos);
    }

    fw64_spritebatch_begin(game->spritebatch);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, "min", game->min_pos.x, game->min_pos.y);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, "max", game->max_pos.x, game->max_pos.y);

    if (game->flags & FLAG_HIT_POS_ACTIVE) {
        fw64_spritebatch_draw_string(game->spritebatch, game->font, "hit", game->hit_pos.x, game->hit_pos.y);
    }

    fw64_spritebatch_draw_sprite(game->spritebatch, game->crosshair, 
        game->crosshair_pos.x - fw64_texture_width(game->crosshair) / 2, game->crosshair_pos.y - fw64_texture_height(game->crosshair) / 2);
    
    fw64_spritebatch_end(game->spritebatch);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);

    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_set_camera(renderpass, &game->arcball_camera.camera);
    fw64_renderpass_begin(renderpass);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, game->spritebatch);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

#define CROSSHAIR_SPEED 100.0f

void move_crosshair(Game* game) {
    Vec2 stick;
    fw64_input_controller_stick(game->engine->input, 0, &stick);

    if (fw64_input_controller_button_down(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        game->crosshair_pos.x -= (int)(CROSSHAIR_SPEED * game->engine->time->time_delta);
    } else if (fw64_input_controller_button_down(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        game->crosshair_pos.x += (int)(CROSSHAIR_SPEED * game->engine->time->time_delta);
    }

    if (fw64_input_controller_button_down(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        game->crosshair_pos.y += (int)(CROSSHAIR_SPEED * game->engine->time->time_delta);
    } else if (fw64_input_controller_button_down(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        game->crosshair_pos.y -= (int)(CROSSHAIR_SPEED * game->engine->time->time_delta);
    }
}
