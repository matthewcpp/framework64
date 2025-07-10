#pragma once

#include "fw64_character.h"

#include <framework64/engine.h>

typedef struct {
    fw64Engine* engine;
    fw64Node* node;
    fw64Character character;
} Player;

void player_init(Player* player, fw64Engine* engine, fw64CharacterEnvironment* env, fw64Scene* scene, fw64Node* node);
void player_update(Player* player);
