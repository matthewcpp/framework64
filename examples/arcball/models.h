#ifndef PENGUIN_H
#define PENGUIN_H

#include "framework64/entity.h"
#include "framework64/renderer.h"
#include "framework64/font.h"
#include "framework64/sprite.h"

typedef struct {
    Entity entity;
} N64Logo;

void n64logo_init(N64Logo* n64logo);
void n64logo_draw(N64Logo* n64logo, Renderer* renderer);


typedef struct {
    Entity entity;
} Suzanne;

void suzanne_init(Suzanne* suzanne);
void suzanne_draw(Suzanne* suzanne, Renderer* renderer);


typedef struct {
    Entity entity;
    int draw_with_lighting;
} Penguin;

void penguin_init(Penguin* penguin);
void penguin_draw(Penguin* penguin, Renderer* renderer);

void init_buttons_sprite(ImageSprite* buttons_sprite);

#endif