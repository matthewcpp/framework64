#ifndef GAME_H
#define GAME_H

#include "arcball_camera.h"
#include "framework64/camera.h"
#include "framework64/font.h"
#include "framework64/system.h"

#include "models.h"

#define ENTITY_COUNT 3

typedef struct {
    fw64System* system;

    Camera camera;
    ArcballCamera arcball;

    Penguin penguin;
    Suzanne suzanne;
    N64Logo n64logo;

    Font consolas;
    ImageSprite button_sprite;

    Entity* entities[ENTITY_COUNT];
    int current_entity;

    int switch_model_text_width;
} Game;

void game_init(Game* game, fw64System* system);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif