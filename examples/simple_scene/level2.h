#pragma once

#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "../common/flame.h"
#include "player.h"
#include "chase_cam.h"
#include "ui.h"

typedef struct {
    fw64Engine* engine;

    fw64Scene* scene;
    UI ui;

    Player player;
    ChaseCamera chase_cam;
    fw64Node* respawn_node;
} Level2;

#ifdef __cplusplus
extern "C" {
#endif

void level2_init(Level2* level, fw64Engine* engine);
void level2_update(Level2* level);
void level2_draw(Level2* level);

#ifdef __cplusplus
}
#endif