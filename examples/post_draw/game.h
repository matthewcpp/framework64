#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

typedef enum {
    DEMO_TYPE_PLASMA,
    DEMO_TYPE_MOIRE,
    
    NUM_DEMO_TYPES
} DemoType;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Node node;
    fw64Mesh* quad;
    int line_num;
    float rotation;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
