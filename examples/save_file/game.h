#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

typedef struct {
    uint64_t magic;
    uint64_t counter;
    uint64_t example_padding[62];
} SaveData;

typedef struct lines_example {
    SaveData save_data;
    fw64Engine* engine;
    fw64Camera camera;
    fw64Font* font;
    float rotation;
    fw64Node solid_cube;
    int loaded;
    int selected_index;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* example, fw64Engine* engine);
void game_update(Game* example);
void game_draw(Game* example);

#ifdef __cplusplus
}
#endif
