#ifndef SPRITES_H
#define SPRITES_H

#include "framework64/renderer.h"
#include "framework64/sprite.h"

typedef struct {
    ImageSprite* sprite;
    IVec2 position;
} N64LogoSprite;

void n64_logo_sprite_init(N64LogoSprite* logo_sprite, ImageSprite* image);
void n64_logo_sprite_draw(N64LogoSprite* logo_sprite, fw64Renderer* renderer);

typedef struct {
    ImageSprite* sprite;
    IVec2 position;
    int frame_index;
    float frame_time;
    float frame_speed;
} KenSprite;

void ken_sprite_init(KenSprite* ken, ImageSprite* image);
void ken_sprite_update(KenSprite* ken, float time_delta);
void ken_sprite_draw(KenSprite* ken, fw64Renderer* renderer);

typedef struct {
    float total_time;
} ElapsedTime;

void elapsed_time_init(ElapsedTime* elapsed_time);
void elapsed_time_update(ElapsedTime* elapsed_time, float time_delta);
void elapsed_time_draw(ElapsedTime* elpased_time, fw64Renderer* renderer, Font* font);

void init_basic_lazer(Font* font);

#endif