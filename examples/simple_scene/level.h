#pragma once

#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
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
} Level;

#ifdef __cplusplus
extern "C" {
#endif

void level_init(Level* level, fw64Engine* engine);
void level_update(Level* level);
void level_draw(Level* level);

#ifdef __cplusplus
}
#endif