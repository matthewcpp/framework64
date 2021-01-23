#ifndef SPRITES_H
#define SPRITES_H

#include "ultra/sprite.h"
#include "ultra/renderer.h"

typedef struct {
    IVec2 position;
    ImageSprite sprite;
} N64LogoSprite;

void n64_logo_sprite_init(N64LogoSprite* logo_sprite);
void n64_logo_sprite_draw(N64LogoSprite* logo_sprite, Renderer* renderer);

typedef struct {
    IVec2 position;
    ImageSprite sprite;
    int frame_index;
    float frame_time;
    float frame_speed;
} KenSprite;

void ken_sprite_init(KenSprite* ken);
void ken_sprite_update(KenSprite* ken, float time_delta);
void ken_sprite_draw(KenSprite* ken, Renderer* renderer);

#endif