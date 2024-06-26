#ifndef SPRITES_H
#define SPRITES_H

#include "framework64/sprite_batch.h"
#include "framework64/texture.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    fw64Texture* sprite;
    IVec2 position;
} N64LogoSprite;

void n64_logo_sprite_init(N64LogoSprite* logo_sprite, fw64Image* image);
void n64_logo_sprite_draw(N64LogoSprite* logo_sprite, fw64SpriteBatch* sprite_batch);

typedef struct {
    fw64Texture* sprite;
    IVec2 position;
    int frame_index;
    float frame_time;
    float frame_speed;
} KenSprite;

void ken_sprite_init(KenSprite* ken, fw64Image* image);
void ken_sprite_update(KenSprite* ken, float time_delta);
void ken_sprite_draw(KenSprite* ken, fw64SpriteBatch* sprite_batch);

typedef struct {
    float total_time;
    fw64Font* font;
} ElapsedTime;

void elapsed_time_init(ElapsedTime* elapsed_time, fw64Font* font);
void elapsed_time_update(ElapsedTime* elapsed_time, float time_delta);
void elapsed_time_draw(ElapsedTime* elapsed_time, fw64SpriteBatch* sprite_batch);

typedef struct {
    const char* string;
    uint32_t str_length;
    uint32_t current_length;
    float current_time;
    fw64Font* font;
} TypewriterText;

void typewriter_text_init(TypewriterText* text, const char* str, fw64Font* font);
void typewriter_text_update(TypewriterText* text, float time_delta);
void typewriter_text_draw(TypewriterText* text, fw64SpriteBatch* sprite_batch);

#ifdef __cplusplus
}
#endif

#endif
