#ifndef GAME_H
#define GAME_H

#include "fps_camera.h"

#include "framework64/billboard.h"
#include "framework64/entity.h"
#include "framework64/font.h"
#include "framework64/input.h"
#include "framework64/system.h"

#include "object.h"

typedef struct {
    System* system;
    FpsCamera fps;
    Entity solid_cube;
    BillboardQuad nintendo_seal_quad;
    ImageSprite nintendo_seal_sprite;
    BillboardQuad n64_logo_quad;
    ImageSprite n64_logo_sprite;
} Game;

void game_init(Game* game, System* system);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif