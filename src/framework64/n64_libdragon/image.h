#pragma once

#include "framework64/image.h"

#include <libdragon.h>

#include <stdint.h>

struct fw64Image {
    uint8_t* buffer;
    sprite_t* libdragon_sprite;
};

int fw64_libdragon_init_image_from_datasource(fw64Image* image, fw64DataSource* data_source, fw64Allocator* allocator);
void fw64_libdragon_image_uninit(fw64Image* image, fw64Allocator* allocator);
