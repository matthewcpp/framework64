#pragma once

#include "framework64/node.h"

#include "framework64/engine.h"

typedef struct {
    fw64Node* entity;
    float update_time_remaining;
    fw64Texture* texture;
    fw64Camera* camera;
} Flame;

#ifdef __cplusplus
extern "C" {
#endif

void flame_init(Flame* flame, fw64Engine* engine, fw64Node* node);
void flame_update(Flame* flame, float time_delta);

#ifdef __cplusplus
}
#endif
