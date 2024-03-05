#include "game.h"
#include "framework64/util/texture_util.h"

#include "assets/assets.h"

#include "framework64/n64/controller_button.h"

#include <limits.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->flags = FLAG_NONE;
    fw64Allocator* allocator = fw64_default_allocator();

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->crosshair = fw64_texture_util_create_from_loaded_image(engine->assets, FW64_ASSET_image_crosshair, allocator);
    fw64_arcball_init(&game->arcball_camera, engine->input, fw64_displays_get_primary(engine->displays));
    
    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 1;
    info.mesh_count = 1;
    info.collider_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_blue_cube, 0);
    fw64Node* node = fw64_scene_get_node(&game->scene, 0);
    fw64Collider* collider = fw64_scene_get_collider(&game->scene, 0);
    fw64_node_set_mesh(node, mesh);
    fw64_node_set_box_collider(node, collider);
    fw64_node_update(node);

    fw64_arcball_set_initial(&game->arcball_camera, &collider->bounding);
}

static void reset_crosshair_position(Game* game) {
    game->flags &= ~FLAG_CROSSHAIR_ACTIVE;
    game->crosshair_pos = fw64_display_get_size(fw64_displays_get_primary(game->engine->displays));
    game->crosshair_pos.x /= 2;
    game->crosshair_pos.y /= 2;
}

void game_update(Game* game){
    if (fw64_input_controller_button_down(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_Z)) {
        game->flags |= FLAG_CROSSHAIR_ACTIVE;

        if (fw64_input_controller_button_down(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_A)) {
            Vec3 ray_origin, ray_direction;
            
            fw64_camera_ray_from_viewport_pos(&game->arcball_camera.camera, &game->crosshair_pos, &ray_origin, &ray_direction);
            if (fw64_scene_raycast(&game->scene, &ray_origin, &ray_direction, UINT32_MAX, &game->raycast)) {
                game->flags |= FLAG_HIT_POS_ACTIVE;
            } else {
                game->flags &= ~FLAG_HIT_POS_ACTIVE;
            }
        }
    }
    else {
        reset_crosshair_position(game);
    }

    fw64_arcball_update(&game->arcball_camera, game->engine->time->time_delta);

    Vec3 min = {-1.0f, -1.0f, -1.0f}, max = {1.0f, 1.0f, 1.0f};
    fw64_camera_world_to_viewport_pos(&game->arcball_camera.camera, &min, &game->min_pos);
    fw64_camera_world_to_viewport_pos(&game->arcball_camera.camera, &max, &game->max_pos);

    if (game->flags & FLAG_HIT_POS_ACTIVE) {
        fw64_camera_world_to_viewport_pos(&game->arcball_camera.camera, &game->raycast.point, &game->hit_pos);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->arcball_camera.camera);
    fw64_scene_draw_all(&game->scene, game->engine->renderer);

    fw64_renderer_draw_text(game->engine->renderer, game->font, game->min_pos.x, game->min_pos.y, "min");
    fw64_renderer_draw_text(game->engine->renderer, game->font, game->max_pos.x, game->max_pos.y, "max");

    if (game->flags & FLAG_HIT_POS_ACTIVE) {
        fw64_renderer_draw_text(game->engine->renderer, game->font, game->hit_pos.x, game->hit_pos.y, "hit");
    }

    if (game->flags & FLAG_CROSSHAIR_ACTIVE) {
        fw64_renderer_draw_sprite(game->engine->renderer, game->crosshair, 
            game->crosshair_pos.x - fw64_texture_width(game->crosshair) / 2, game->crosshair_pos.y - fw64_texture_height(game->crosshair) / 2);
    }

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
