#ifndef RENDERER_H
#define RENDERER_H

#include <nusys.h>

#include "camera.h"
#include "color.h"
#include "entity.h"
#include "Rect.h"
#include "sprite.h"
#include "vec2.h"
#include "font.h"

/* The maximum length of the display list of one task  */
#define GFX_GLIST_LEN     2048

typedef struct {
    // holds the current command insertion point of the display list
    Gfx* display_list;

    // display list for drawing commands
    Gfx gfx_list[GFX_GLIST_LEN];

    Color fill_color;

    Vp view_port;
    IVec2 screen_size;
} Renderer;


void renderer_init(Renderer* renderer, int screen_width, int screen_height);
void renderer_begin(Renderer* renderer, Camera* camera);
void renderer_end(Renderer* renderer);

void renderer_entity_start(Renderer* renderer, Entity* entity);
void renderer_entity_end(Renderer* renderer);

void renderer_begin_2d(Renderer* renderer);
void renderer_set_fill_color(Renderer* renderer, Color* color);
void renderer_set_fill_mode(Renderer* renderer);
void renderer_draw_filled_rect(Renderer* renderer, IRect* rect);
void renderer_set_sprite_mode(Renderer* renderer);

void renderer_draw_sprite(Renderer* renderer, ImageSprite* sprite, int x, int y);
void renderer_draw_sprite_slice(Renderer* renderer, ImageSprite* sprite, int frame, int x, int y);
void renderer_draw_text_sprite(Renderer* renderer, TextSprite* text_sprite, int x, int y);

void renderer_get_screen_size(Renderer* renderer, IVec2* screen_size);

#endif