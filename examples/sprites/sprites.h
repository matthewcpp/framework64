#ifndef SPRITES_H
#define SPRITES_H

#include "framework64/renderer.h"
#include "framework64/texture.h"

typedef struct {
    fw64Texture* sprite;
    IVec2 position;
} N64LogoSprite;

void n64_logo_sprite_init(N64LogoSprite* logo_sprite, fw64Texture* image);
void n64_logo_sprite_draw(N64LogoSprite* logo_sprite, fw64Renderer* renderer);

typedef struct {
    fw64Texture* sprite;
    IVec2 position;
    int frame_index;
    float frame_time;
    float frame_speed;
} KenSprite;

void ken_sprite_init(KenSprite* ken, fw64Texture* image);
void ken_sprite_update(KenSprite* ken, float time_delta);
void ken_sprite_draw(KenSprite* ken, fw64Renderer* renderer);

typedef struct {
    float total_time;
    fw64Font* font;
} ElapsedTime;

void elapsed_time_init(ElapsedTime* elapsed_time, fw64Font* font);
void elapsed_time_update(ElapsedTime* elapsed_time, float time_delta);
void elapsed_time_draw(ElapsedTime* elapsed_time, fw64Renderer* renderer);

#endif