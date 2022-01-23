#include "game.h"
#include "assets.h"

#include "framework64/n64/controller_button.h"
#include "framework64/util/quad.h"

static void draw_frame(fw64Framebuffer* framebuffer, void* arg);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    fw64_renderer_set_anti_aliasing_enabled(game->engine->renderer, 0);

    game->line_num = 0;
    game->rotation = 0.0f;

    game->quad = fw64_textured_quad_create(game->engine, FW64_ASSET_image_n64_logo, NULL);
    fw64_node_init(&game->node);
    fw64_node_set_mesh(&game->node, game->quad);

    fw64_renderer_set_post_draw_callback(game->engine->renderer, draw_frame, game);
}


void game_update(Game* game){
    game->rotation += game->engine->time->time_delta * 90.0f;
    quat_from_euler(&game->node.transform.rotation, 0.0f, 0.0f, game->rotation);
    fw64_node_update(&game->node);
}

void game_draw(Game* game) {
    IVec2 screen_size;
    fw64_renderer_get_screen_size(game->engine->renderer, &screen_size);

    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->node.transform, game->quad);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void draw_frame(fw64Framebuffer* framebuffer, void* arg){
    Game* game = (Game*) arg;
    IVec2 screen_size;
    fw64_renderer_get_screen_size(game->engine->renderer, &screen_size);

    for (int x = 0; x < screen_size.x; x++) {
        fw64_framebuffer_set_pixel(framebuffer, x, game->line_num, 255, 0, 0);
    }
    
    game->line_num += 1;

    if (game->line_num >= screen_size.y)
        game->line_num = 0;
}