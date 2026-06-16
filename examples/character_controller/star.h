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
    Vec3 initial_pos;
    Vec3 initial_scale;
} Star;

void star_init(Star* star, fw64Node* node, Player* player, StarTriggerFunc trigger_func, void* trigger_arg);
void star_update(Star* star, float time_delta);
void star_reset(Star* star);

#define star_get_node(star) ((star)->rotate.node)
