#include "framework64/n64/fill_rect.h"

#include <string.h>

#define FW64_N64_FILL_RECT_IMAGE_SIZE 2 * 4

uint8_t fill_rect_mem[FW64_N64_FILL_RECT_IMAGE_SIZE] __attribute__ ((aligned (8)));

void n64_fill_rect_init(N64FillRect* fill_rect) {
    fill_rect->image.info.format = FW64_N64_IMAGE_FORMAT_RGBA32;
    fill_rect->image.info.width = 2;
    fill_rect->image.info.height = 1;
    fill_rect->image.info.hslices = 1;
    fill_rect->image.info.vslices = 1;
    fill_rect->image.rom_addr = 0;
    fill_rect->image.data = &fill_rect_mem[0];

    memset(&fill_rect_mem[0], 255, FW64_N64_FILL_RECT_IMAGE_SIZE);
    
    fw64_n64_texture_init_with_image(&fill_rect->texture, &fill_rect->image);
    fw64_texture_set_wrap_mode(&fill_rect->texture, FW64_TEXTURE_WRAP_CLAMP, FW64_TEXTURE_WRAP_CLAMP);
}
