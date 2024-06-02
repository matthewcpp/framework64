#include "game.h"
#include "assets/assets.h"

#include "framework64/random.h"

#include "framework64/controller_mapping/n64.h"

#define TILE_COUNT 6
#define BUMP_ALLOCATOR_SIZE (16 * 1024)
#define TILE_SIZE 100.0f

int tile_scenes[TILE_COUNT] = {
    FW64_ASSET_scene_tile_arches,
    FW64_ASSET_scene_tile_cubes,
    FW64_ASSET_scene_tile_trees,
    FW64_ASSET_scene_tile_cylinders,
    FW64_ASSET_scene_tile_spheres,
    FW64_ASSET_scene_tile_torus
};

static void tiles_test_load_tile(Game* game, int index, Vec3* pos);
static void tiles_test_load_next_row(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->next_row_pos = 0.0f;
    game->handle_index = 0;

    fw64_level_init(&game->level, engine);

    for (int i = 0; i < ACTIVE_TILE_COUNT; i++) {
        game->chunk_handles[i] = FW64_LEVEL_INVALID_CHUNK_HANDLE;
        fw64_bump_allocator_init(&game->allocators[i], BUMP_ALLOCATOR_SIZE);
    }
    fw64_fps_camera_init(&game->fps_camera, engine->input, fw64_displays_get_primary(engine->displays));
    game->fps_camera.movement_speed = 30.0f;
    game->fps_camera.camera.near = 1.0f;
    game->fps_camera.camera.far = 250.0f;
    fw64_camera_update_projection_matrix(&game->fps_camera.camera);

    vec3_set(&game->fps_camera.camera.transform.position, 0.0f, 6.0f, 50.0f);
    game->player_previous_z = game->fps_camera.camera.transform.position.z;
        
    tiles_test_load_next_row(game);
    tiles_test_load_next_row(game);
    game->next_row_trigger = -TILE_SIZE / 2.0f;

    fw64_renderer_set_clear_color(engine->renderer, 147, 204, 234);
}

void tiles_test_load_next_row(Game* game) {
    Vec3 pos = {-TILE_SIZE, 0.0f, game->next_row_pos};
    tiles_test_load_tile(game, game->handle_index++, &pos);

    pos.x = 0.0f;
    tiles_test_load_tile(game, game->handle_index++, &pos);

    pos.x = TILE_SIZE;
    tiles_test_load_tile(game, game->handle_index++, &pos);

    game->next_row_pos -= TILE_SIZE;
    game->next_row_trigger -= TILE_SIZE;

    if (game->handle_index >= ACTIVE_TILE_COUNT)
        game->handle_index = 0;
}

void tiles_test_load_tile(Game* game, int index, Vec3* pos) {
    // eject previous chunk in this index
    fw64_level_unload_chunk(&game->level, game->chunk_handles[index]);
    fw64_bump_allocator_reset(&game->allocators[index]);

    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);
    info.scene_id = tile_scenes[fw64_random_int_in_range(0, TILE_COUNT - 1)];
    info.allocator = &game->allocators[index].interface;

    game->chunk_handles[index] = fw64_level_load_chunk_at_pos(&game->level, &info, pos);
}

void game_update(Game* game){
    fw64_level_update(&game->level);
    fw64_fps_camera_update(&game->fps_camera, game->engine->time->time_delta);

    float player_z = game->fps_camera.camera.transform.position.z;

    if (game->player_previous_z > game->next_row_trigger &&  player_z <= game->next_row_trigger)
        tiles_test_load_next_row(game);

    game->player_previous_z = player_z;
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->fps_camera.camera);
    fw64_level_draw_camera(&game->level, &game->fps_camera.camera);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
