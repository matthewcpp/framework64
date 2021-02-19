#include "sprites.h"

#include "assets.h"

#include <malloc.h>

void n64_logo_sprite_init(N64LogoSprite* logo_sprite, ImageSprite* image) {
    logo_sprite->sprite = image;
    logo_sprite->position.x = 0;
    logo_sprite->position.y = 0;
}

void n64_logo_sprite_draw(N64LogoSprite* logo_sprite, Renderer* renderer){
    renderer_draw_sprite(renderer, logo_sprite->sprite, logo_sprite->position.x, logo_sprite->position.y);
}

#define FRAME_DUR 1.0f / 10.0f

void ken_sprite_init(KenSprite* ken, ImageSprite* image) {
    ken->sprite = image;
    ken->position.x = 0;
    ken->position.y = 0;
    ken->frame_time = 0.0f;
    ken->frame_speed = 1.0f;
    ken->frame_index = 0;
}

void ken_sprite_update(KenSprite* ken, float time_delta) {
    ken->frame_time += time_delta;

    if (ken->frame_time >= FRAME_DUR) {
        ken->frame_time -= FRAME_DUR;

        ken->frame_index += 1;
        if (ken->frame_index >= ken->sprite->hslices)
            ken->frame_index = 0;
    }

}

void ken_sprite_draw(KenSprite* ken, Renderer* renderer) {
    int slice_height = image_sprite_get_slice_height(ken->sprite);

    renderer_draw_sprite_slice(renderer, ken->sprite, ken->frame_index, ken->position.x, ken->position.y);
    renderer_draw_sprite_slice(renderer, ken->sprite, ken->frame_index + ken->sprite->hslices, ken->position.x, ken->position.y + slice_height);
}

void elapsed_time_init(ElapsedTime* elapsed_time) {
    elapsed_time->total_time = 0.0f;
}

void elapsed_time_update(ElapsedTime* elapsed_time, float time_delta) {
    elapsed_time->total_time += time_delta;
}

void elapsed_time_draw(ElapsedTime* elpased_time, Renderer* renderer, Font* font) {
    char elapsed_time_text[24];
    sprintf(elapsed_time_text, "ELAPSED TIME: %.2f", elpased_time->total_time);

    renderer_draw_text(renderer, font, 200, 10, elapsed_time_text);
}
