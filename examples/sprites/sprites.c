#include "sprites.h"

#include "assets/assets.h"

#include <stdio.h>
#include <string.h>

void n64_logo_sprite_init(N64LogoSprite* logo_sprite, fw64Image* image) {
    logo_sprite->sprite = fw64_texture_create_from_image(image, fw64_default_allocator());
    logo_sprite->position.x = 0;
    logo_sprite->position.y = 0;
}

void n64_logo_sprite_draw(N64LogoSprite* logo_sprite, fw64SpriteBatch* sprite_batch){
    fw64_spritebatch_draw_sprite(sprite_batch, logo_sprite->sprite, logo_sprite->position.x, logo_sprite->position.y);
}

#define FRAME_DUR 1.0f / 10.0f

void ken_sprite_init(KenSprite* ken, fw64Image* image) {
    ken->sprite = fw64_texture_create_from_image(image, fw64_default_allocator());
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
        if (ken->frame_index >= fw64_texture_hslices(ken->sprite))
            ken->frame_index = 0;
    }

}

void ken_sprite_draw(KenSprite* ken, fw64SpriteBatch* sprite_batch) {
    int slice_height = fw64_texture_slice_height(ken->sprite);

    fw64_spritebatch_draw_sprite_slice(sprite_batch, ken->sprite, ken->frame_index, ken->position.x, ken->position.y);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ken->sprite, ken->frame_index + fw64_texture_hslices(ken->sprite), ken->position.x, ken->position.y + slice_height);
}

void elapsed_time_init(ElapsedTime* elapsed_time, fw64Font* font) {
    elapsed_time->total_time = 0.0f;
    elapsed_time->font = font;
}

void elapsed_time_update(ElapsedTime* elapsed_time, float time_delta) {
    elapsed_time->total_time += time_delta;
}

void elapsed_time_draw(ElapsedTime* elapsed_time, fw64SpriteBatch* sprite_batch) {
    char elapsed_time_text[24];
    sprintf(elapsed_time_text, "ELAPSED TIME: %.2f", elapsed_time->total_time);

    fw64_spritebatch_draw_string(sprite_batch, elapsed_time->font, elapsed_time_text, 200, 10);
}

#define CHARACTER_TIME 0.33f

void typewriter_text_init(TypewriterText* text, const char* str, fw64Font* font) {
    text->string = str;
    text->str_length = (uint32_t)strlen(str);
    text->font = font;

    text->current_length = 1;
    text->current_time = 0.0f;
}
void typewriter_text_update(TypewriterText* text, float time_delta) {
    text->current_time += time_delta;

    if (text->current_time >= CHARACTER_TIME) {
        text->current_length += 1;
        text->current_time -= CHARACTER_TIME;

        if (text->current_length > text->str_length) {
            text->current_length = 1;
        }
    }
}

void typewriter_text_draw(TypewriterText* text, fw64SpriteBatch* sprite_batch) {
    fw64_spritebatch_draw_string_count(sprite_batch, text->font, text->string, text->current_length, 5, 150);
}
