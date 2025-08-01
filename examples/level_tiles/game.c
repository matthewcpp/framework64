#include "game.h"
#include "assets/assets.h"
#include "assets/layers.h"

#include "framework64/collision.h"
#include "framework64/log.h"
#include "framework64/math.h"
#include "framework64/random.h"

#include "framework64/controller_mapping/n64.h"

void player_init(Player* player, fw64Engine* engine, Tiles* level);
void player_update(Player* player);

void follow_camera_init(FollowCamera* follow_cam, fw64Engine* engine, Tiles* tiles, fw64Node* target);
void follow_camera_update(FollowCamera* follow_cam);

void tiles_init(Tiles* tiles, fw64Engine* engine, fw64SceneInfo* persistent_info);
void tiles_load_next_tile(Tiles* tiles, int tile_index);
void tiles_load_next_random_tile(Tiles* tiles);
void tiles_draw(Tiles* tiles, fw64RenderPass* renderpass, fw64Frustum* frustum);
uint32_t tiles_query(Tiles* tiles, Box* box, Vec3* velocity, fw64IntersectMovingBoxQuery* query);

#define BUMP_ALLOCATOR_SIZE (16 * 1024)
#define SCENE_COUNT 6
#define TILE_OFFSET_AMOUNT 100.0f

#define PLAYER_DEFAULT_RUN_SPEED 66.6f
#define PLAYER_STRAFE_SPEED 60.0f
#define STICK_THRESHOLD 0.1f

int tile_scenes[SCENE_COUNT] = {
    FW64_ASSET_scene_tile_arches,
    FW64_ASSET_scene_tile_cubes,
    FW64_ASSET_scene_tile_trees,
    FW64_ASSET_scene_tile_cylinders,
    FW64_ASSET_scene_tile_spheres,
    FW64_ASSET_scene_tile_torus
};

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->renderpass = fw64_renderpass_create(display, allocator);

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 2;
    info.collider_count = 1;
    info.skinned_mesh_count = 1;
    info.skinned_mesh_instance_count = 1;
    tiles_init(&game->tiles, engine, &info);

    player_init(&game->player, engine, &game->tiles);
    follow_camera_init(&game->follow_camera, engine, &game->tiles, game->player.node);

    Vec3 forward;
    fw64_transform_back(&game->follow_camera.camera.node->transform, &forward);
    fw64_renderpass_set_light_direction(game->renderpass, 0, &forward);

    tiles_load_next_tile(&game->tiles, 3);
    tiles_load_next_random_tile(&game->tiles);
    tiles_load_next_random_tile(&game->tiles);
}

void player_init(Player* player, fw64Engine* engine, Tiles* tiles) {
    player->engine = engine;
    player->node = fw64_scene_create_node(tiles->persistent);
    player->tiles = tiles;
    player->speed = PLAYER_DEFAULT_RUN_SPEED;

    vec3_set_all(&player->node->transform.scale, 0.05f);
    fw64_node_update(player->node);

    fw64SkinnedMesh* skinned_mesh = fw64_scene_load_skinned_mesh_asset(tiles->persistent, FW64_ASSET_skinnedmesh_catherine);
    player->skinned_mesh = fw64_scene_create_skinned_mesh_instance(tiles->persistent, player->node, skinned_mesh, 0);
    player->skinned_mesh->controller.loop = 1;
    fw64_animation_controller_play(&player->skinned_mesh->controller);

    Box bounding = fw64_mesh_get_bounding_box(skinned_mesh->mesh);
    fw64_scene_create_box_collider(tiles->persistent, player->node, &bounding);
}

void player_update(Player* player) {
    Vec2 stick;
    fw64_input_controller_stick(player->engine->input, 0, &stick);

    player->prev_pos = player->node->transform.position;
    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        if (player->speed > 0.0f) {
            fw64_animation_controller_pause(&player->skinned_mesh->controller);
            player->speed = 0.0f;
        } else {
            fw64_animation_controller_play(&player->skinned_mesh->controller);
            player->speed = PLAYER_DEFAULT_RUN_SPEED;
        }
    }

    player->node->transform.position.z += player->speed * player->engine->time->time_delta;

    if (stick.x > STICK_THRESHOLD) {
        float x = player->node->transform.position.x - (PLAYER_STRAFE_SPEED * player->engine->time->time_delta);
        player->node->transform.position.x = fw64_minf(x, TILE_OFFSET_AMOUNT / 2.0f);
    } else if (stick.x < -STICK_THRESHOLD) {
        float x = player->node->transform.position.x + (PLAYER_STRAFE_SPEED * player->engine->time->time_delta);
        player->node->transform.position.x = fw64_maxf(x, -TILE_OFFSET_AMOUNT / 2.0f);
    }

    fw64_node_update(player->node);

    Vec3 velocity;
    vec3_subtract(&player->node->transform.position, &player->prev_pos, &velocity);

    fw64IntersectMovingBoxQuery query;
    fw64_intersect_moving_box_query_init(&query);
    if (tiles_query(player->tiles, &player->node->collider->bounding, &velocity, &query)) {
        for (int i = 0; i < query.count; i++) {
            if (query.results[i].node->layer_mask & FW64_layer_triggers) {
                fw64_collider_deactivate(query.results[i].node->collider);
                tiles_load_next_random_tile(player->tiles);
            }
        }
    }

    fw64_skinned_mesh_instance_update(player->skinned_mesh, player->engine->time->time_delta);
}

void tiles_init(Tiles* tiles, fw64Engine* engine, fw64SceneInfo* persistent_info) {
    fw64Allocator* allocator = fw64_default_allocator();
    tiles->engine = engine;
    tiles->persistent = fw64_allocator_malloc(allocator, sizeof(fw64Scene));
    fw64_scene_init(tiles->persistent, persistent_info, engine->assets, allocator);

    vec3_set(&tiles->next_scene_pos, 0.0f, 0.0f, 50.0f);
    tiles->next_tile_index = 0;

    for (int i = 0; i < TILE_COUNT; i++) {
        tiles->tiles[i].scene = NULL;
        fw64_bump_allocator_init(&tiles->tiles[i].allocator, BUMP_ALLOCATOR_SIZE);
    }
}

uint32_t tiles_query(Tiles* tiles, Box* box, Vec3* velocity, fw64IntersectMovingBoxQuery* query) {
    for (int i = 0; i < TILE_COUNT; i++) {
        Tile* tile = tiles->tiles + i;

        if (!tile->scene) {
            continue;
        }

        fw64_scene_moving_box_intersection(tile->scene, box, velocity, ~0U, query);
    }

    return query->count;
}

void tiles_load_next_random_tile(Tiles* tiles) {
    tiles_load_next_tile(tiles, fw64_random_int_in_range(0, SCENE_COUNT - 1));
}

void tiles_load_next_tile(Tiles* tiles, int scene_index){
    Tile* tile = tiles->tiles + tiles->next_tile_index;

    if (tile->scene) {
        fw64_scene_delete(tile->scene);
        fw64_bump_allocator_reset(&tile->allocator);
    }

    tile->index = scene_index;
    tile->scene = fw64_assets_load_scene(tiles->engine->assets, tile_scenes[scene_index], &tile->allocator.interface);
    fw64Node* root_node = fw64_scene_get_node(tile->scene, 0);
    vec3_add(&root_node->transform.position, &tiles->next_scene_pos, &root_node->transform.position);
    
    // update transform and components
    // TODO: is there a less manual way to accomplish this?
    fw64_node_update(root_node);
    for (size_t i = 0; i < fw64_scene_get_collider_count(tile->scene); i++) {
        fw64_collider_update(fw64_scene_get_collider(tile->scene, i));
    }

    for (size_t i = 0; i < fw64_scene_get_mesh_instance_count(tile->scene); i++) {
        fw64_mesh_instance_update(fw64_scene_get_mesh_instance(tile->scene, i));
    }

    fw64_scene_update_bounding(tile->scene);

    tiles->next_scene_pos.z += TILE_OFFSET_AMOUNT;
    tiles->next_tile_index += 1;

    if (tiles->next_tile_index == TILE_COUNT) {
        tiles->next_tile_index = 0;
    }
}

void tiles_draw(Tiles* tiles, fw64RenderPass* renderpass, fw64Frustum* frustum) {
    fw64_scene_draw_frustrum(tiles->persistent, renderpass, frustum, ~0U);

    for (int i = 0; i < TILE_COUNT; i++) {
        if (tiles->tiles[i].scene != NULL) {
            fw64_scene_draw_frustrum(tiles->tiles[i].scene, renderpass, frustum, ~0U);
        }
    }
}

void follow_camera_init(FollowCamera* follow_cam, fw64Engine* engine, Tiles* tiles, fw64Node* target) {
    fw64Node* camera_node = fw64_scene_create_node(tiles->persistent);
    fw64_camera_init(&follow_cam->camera, camera_node, fw64_displays_get_primary(engine->displays));
    follow_cam->target = target;
    vec3_set(&follow_cam->offset, 0.0f, 15.0f, -40.0f);
}

void follow_camera_update(FollowCamera* follow_cam) {
    vec3_add(&follow_cam->target->transform.position, &follow_cam->offset, &follow_cam->camera.node->transform.position);
    follow_cam->camera.node->transform.position.x = 0.0f;
    
    Vec3 target, extents, up = {0.0f, 1.0f, 0.0f};
    box_center(&follow_cam->target->collider->bounding, &target);
    box_extents(&follow_cam->target->collider->bounding, &extents);
    target.x = 0.0f;
    target.y += extents.y;
    fw64_transform_look_at(&follow_cam->camera.node->transform, &target, &up);
    fw64_camera_update_view_matrix(&follow_cam->camera);
}

void game_update(Game* game){
    player_update(&game->player);
    follow_camera_update(&game->follow_camera);
}

void game_draw(Game* game) {
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(&game->follow_camera.camera, &frustum);

    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_set_camera(game->renderpass, &game->follow_camera.camera);
    tiles_draw(&game->tiles, game->renderpass, &frustum);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
}
