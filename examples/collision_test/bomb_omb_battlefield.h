#pragma once

#include "player.h"

#include <framework64/engine.h>
#include <framework64/scene.h>
#include <framework64/util/bump_allocator.h>

#include "fw64_headlight.h"
#include "fw64_skybox.h"
#include "fw64_collision_scene_manager.h"
#include "star.h"
#include "star_collected.h"

typedef struct {
    Player player;
    fw64CharacterEnvironment character_environment;
    fw64CollisionSceneManager collision_scene_manager;
    fw64Headlight headlight;
    fw64Skybox skybox;
    StarCollected star_collected;
    Star star;
    fw64Engine* engine;
} BombOmbBattlefield;

void bomb_omb_battlefield_init(BombOmbBattlefield* battlefield, fw64Engine* engine, fw64Allocator* allocator);
void bomb_omb_battlefield_update(BombOmbBattlefield* battlefield);
void bomb_omb_battlefield_fixed_update(BombOmbBattlefield* battlefield);
void bomb_omb_battlefield_draw(BombOmbBattlefield* battlefield);
