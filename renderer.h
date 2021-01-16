#ifndef RENDERER_H
#define RENDERER_H

#include <nusys.h>

#include "camera.h"
#include "color.h"
#include "entity.h"
#include "Rect.h"
#include "texture.h"
#include "vec2.h"

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


Renderer* renderer_create(int screen_width, int screen_height);
void renderer_begin(Renderer* renderer, Camera* camera);
void renderer_end(Renderer* renderer);

void renderer_draw_static(Renderer* renderer, Entity* entity);

void renderer_begin_2d(Renderer* renderer);
void renderer_set_fill_color(Renderer* renderer, Color* color);
void renderer_set_fill_mode(Renderer* renderer);
void renderer_draw_filled_rect(Renderer* renderer, Rect* rect);
void renderer_set_sprite_mode(Renderer* renderer);
void renderer_draw_sprite(Renderer* renderer, Texture* sprite, int x, int y);

void renderer_get_screen_size(Renderer* renderer, IVec2* screen_size);

#endif