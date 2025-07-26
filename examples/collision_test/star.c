#include "star.h"

void star_init(Star* star, fw64Node* node, Player* player, StarTriggerFunc trigger_func, void* trigger_arg){
    fw64_rotate_node_init(&star->rotate, node);
    star->rotate.speed = FW64_ROTATE_NODE_DEFAULT_SPEED * 1.75f;
    star->player = player;
    star->triggered = 0;

    star->trigger_func = trigger_func;
    star->trigger_arg = trigger_arg;
}

#define STAR_RADIUS 2.0f

void star_update(Star* star, float time_delta) {
    fw64_rotate_node_update(&star->rotate, time_delta);

    if (star->triggered) {
        return;
    }

    if (vec3_distance_squared(&star->rotate.node->transform.position, &star->player->node->transform.position) <= STAR_RADIUS * STAR_RADIUS) {
        star->rotate.speed = FW64_ROTATE_NODE_DEFAULT_SPEED * 3.0f;
        star->triggered = 1;

        if (star->trigger_func) {
            star->trigger_func(star->trigger_arg);
        }
    }
}
