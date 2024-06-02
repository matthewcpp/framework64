#pragma once 

#include "framework64/color.h"
#include "framework64/n64_libultra/image.h"
#include "framework64/n64_libultra/texture.h"

typedef struct {
    fw64Image image;
    fw64Texture texture;

} N64FillRect;

void n64_fill_rect_init(N64FillRect* fill_rect);