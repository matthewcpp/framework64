#pragma once

#include "framework64/node.h"

#include "framework64/engine.h"

typedef struct {
    fw64Node entity;
    float update_time_remaining;
} Flame;

#ifdef __cplusplus
extern "C" {
#endif

void flame_init(Flame* flame, fw64Engine* engine);
void flame_update(Flame* flame, float time_delta);
void flame_draw(Flame* flame, fw64Renderer* renderer);

#ifdef __cplusplus
}
#endif