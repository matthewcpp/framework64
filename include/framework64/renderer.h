#ifndef FW64_RENDERER_H
#define FW64_RENDERER_H

#include <nusys.h>

#include "billboard.h"
#include "camera.h"
#include "color.h"
#include "entity.h"
#include "mesh.h"
#include "Rect.h"
#include "sprite.h"
#include "vec2.h"
#include "font.h"

/* The maximum length of the display list of one task  */
#define GFX_GLIST_LEN     2048

typedef enum {
    RENDERER_MODE_UNSET,
    RENDERER_MODE_TRIANGLES,
    RENDERER_MODE_LINES,
    RENDERER_MODE_RECTANGLES
} RenderMode;

typedef enum {
    RENDERER_FLAG_NONE,
    RENDERER_FLAG_NOCLEAR = 0,
    RENDERER_FLAG_CLEAR = 1,

    RENDERER_FLAG_NOSWAP = 0,
    RENDERER_FLAG_SWAP = 2,
} RendererFlags;



typedef struct {
    // holds the current command insertion point of the display list
    Gfx* display_list;
    Gfx* display_list_start;

    // display list for drawing commands
    Gfx gfx_list[GFX_GLIST_LEN];

    Camera* camera;

    u16 fill_color;
    u16 clear_color;

    Vp view_port;
    IVec2 screen_size;
    RenderMode render_mode;
    ShadingMode shading_mode;
} Renderer;


void renderer_init(Renderer* renderer, int screen_width, int screen_height);
void renderer_begin(Renderer* renderer, Camera* camera, RenderMode render_mode, RendererFlags flags);
void renderer_set_clear_color(Renderer* renderer, Color* color);
void renderer_end(Renderer* renderer, RendererFlags flags);

void renderer_entity_start(Renderer* renderer, Entity* entity);
void renderer_entity_end(Renderer* renderer);
void renderer_draw_billboard_quad(Renderer* renderer, BillboardQuad* quad);
void renderer_draw_static_mesh(Renderer* renderer, Transform* transform, Mesh* mesh);

void renderer_set_fill_color(Renderer* renderer, Color* color);
void renderer_set_fill_mode(Renderer* renderer);
void renderer_draw_filled_rect(Renderer* renderer, IRect* rect);

void renderer_draw_sprite(Renderer* renderer, ImageSprite* sprite, int x, int y);
void renderer_draw_sprite_slice(Renderer* renderer, ImageSprite* sprite, int frame, int x, int y);
void renderer_draw_text(Renderer* renderer, Font* font, int x, int y, char* text);

void renderer_get_screen_size(Renderer* renderer, IVec2* screen_size);

#endif