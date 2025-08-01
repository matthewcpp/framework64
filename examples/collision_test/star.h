#pragma once

#include "fw64_rotate_node.h"
#include "player.h"

typedef void (*StarTriggerFunc)(void* arg);

typedef struct {
    fw64RotateNode rotate;
    Player* player;
    int triggered;
    StarTriggerFunc trigger_func;
    void* trigger_arg;
} Star;

void star_init(Star* star, fw64Node* node, Player* player, StarTriggerFunc trigger_func, void* trigger_arg);
void star_update(Star* star, float time_delta);

#define star_get_node(star) ((star)->rotate.node)
