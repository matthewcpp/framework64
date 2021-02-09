#include "data_example.h"

#include "assets.h"

#include "framework64/filesystem.h"

#include <string.h>

char tempbuff[32];

void data_example_init(DataExample* example, Input* input, Renderer* renderer) {
    example->input = input;
    example->renderer = renderer;
    camera_init(&example->camera);

    font_load(ASSET_font_Consolas12, &example->consolas);

    int handle = filesystem_open(ASSET_raw_sample);

    memset(tempbuff, 0, 32);
    int bytes_read = filesystem_read(tempbuff, 1, 5000, handle);

    filesystem_close(handle);
}

void data_example_update(DataExample* example, float time_delta){
}

void data_example_draw(DataExample* example) {
    renderer_begin(example->renderer, &example->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    renderer_begin_2d(example->renderer);
    renderer_set_sprite_mode(example->renderer);
    renderer_draw_text(example->renderer, &example->consolas, 10, 10, tempbuff);
    renderer_end(example->renderer, RENDERER_FLAG_SWAP);
}
