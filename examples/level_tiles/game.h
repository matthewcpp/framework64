#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "framework64/util/bump_allocator.h"

#define TILE_COUNT 4

typedef struct {
    fw64Scene* scene;
    fw64BumpAllocator allocator;
    int index;
} Tile;

typedef struct {
    fw64Engine* engine;
    fw64Scene* persistent;
    Vec3 next_scene_pos;
    int next_tile_index;
    Tile tiles[TILE_COUNT];
} Tiles;

typedef struct {
    fw64Engine* engine;
    fw64Node* node;
    fw64SkinnedMeshInstance* skinned_mesh;
    Tiles* tiles;
    Vec3 prev_pos;
    float speed;
} Player;

typedef struct {
    fw64Node* target;
    fw64Camera camera;
    Vec3 offset;
} FollowCamera;

typedef struct {
    fw64Engine* engine;
    Tiles tiles;
    Player player;
    FollowCamera follow_camera;
    fw64RenderPass* renderpass;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
