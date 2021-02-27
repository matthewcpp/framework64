#ifndef GAME_H
#define GAME_H

#include "framework64/util/arcball_camera.h"
#include "framework64/camera.h"
#include "framework64/entity.h"
#include "framework64/system.h"

#define ENTITY_COUNT 3

typedef struct {
    System* system;
    ArcballCamera arcball;

    Font* consolas;
    ImageSprite* button_sprite;

    Entity entity;
    int current_entity;
    uint32_t mesh_assets[ENTITY_COUNT];

    int switch_model_text_width;
} Game;

void game_init(Game* game, System* system);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif