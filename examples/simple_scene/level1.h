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

    Flame flames[2];
    fw64Node* respawn_node;
} Level1;

#ifdef __cplusplus
extern "C" {
#endif

void level1_init(Level1* level, fw64Engine* engine);
void level1_update(Level1* level);
void level1_draw(Level1* level);

#ifdef __cplusplus
}
#endif