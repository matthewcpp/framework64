#include "level1.h"

#include "assets.h"

#define EXTRA_TYPE_START 0
#define EXTRA_TYPE_FIRE 1

void level1_init(Level1* level, fw64Engine* engine) {
    level->engine = engine;
    chase_camera_init(&level->chase_cam, engine);

    level->scene = fw64_scene_load(level->engine->assets, FW64_ASSET_scene_simple_scene);

    int flame_index = 0;
    fw64Image* flame_image = fw64_image_load(level->engine->assets, FW64_ASSET_image_fire_sprite);

    uint32_t node_count = fw64_scene_get_node_count(level->scene);

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(level->scene, i);

        if (node->type == EXTRA_TYPE_START) {
            level->respawn_node = node;
            player_init(&level->player, level->engine, level->scene, FW64_ASSET_mesh_penguin, NULL);
            vec3_set(&level->player.node.transform.scale, 0.01f, 0.01f, 0.01f);
            level->player.node.transform.position = node->transform.position;
            fw64_node_refresh(&level->player.node);

            level->chase_cam.target = &level->player.node.transform;
        }
        else if (node->type == EXTRA_TYPE_FIRE) {
            Flame* flame = &level->flames[flame_index++];
            flame_init(flame, level->engine, flame_image);
            vec3_set(&flame->entity.transform.scale, 3.0f, 5.0f, 3.0f);
            flame->entity.transform.position = node->transform.position;
            flame->entity.transform.position.y += 3.0f;
        }
    }

    ui_init(&level->ui, engine, &level->player);
}

void level1_update(Level1* level) {
    player_update(&level->player);
    chase_camera_update(&level->chase_cam);

    if (level->player.node.transform.position.y < -10.0f) { // respawn player
        player_reset(&level->player, &level->respawn_node->transform.position);
    }

    flame_update(&level->flames[0], level->engine->time->time_delta);
    flame_update(&level->flames[1], level->engine->time->time_delta);

    ui_update(&level->ui);
}


void level1_draw(Level1* level) {
    fw64Renderer* renderer = level->engine->renderer;

    fw64_renderer_begin(renderer, &level->chase_cam.camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_scene_draw_all(level->scene, renderer);
    player_draw(&level->player);
    flame_draw(&level->flames[0], renderer);
    flame_draw(&level->flames[1], renderer);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_NOSWAP);

    fw64_renderer_begin(renderer, &level->chase_cam.camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_NOCLEAR);
    ui_draw(&level->ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}