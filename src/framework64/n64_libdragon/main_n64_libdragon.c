#if 1
#include "framework64/n64_libdragon/libdragon_engine.h"

#include <libdragon.h>

#include "${game_include_path}"

fw64Engine engine;
Game game;

int main()
{
    fw64_n64_libdragon_engine_init(&engine);
    game_init(&game, &engine);
    
    while (1)
    {
        fw64_n64_libdragon_engine_update(&engine);
        game_update(&game);
        game_draw(&game);
    }
}
#endif

#if 0
#include <libdragon.h>

extern FILE *must_fopen(const char *fn);
extern int must_open(const char *fn);

#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>

int main() 
{
    debug_init_isviewer();
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    dfs_init(DFS_DEFAULT_LOCATION);

    int f = must_open("rom:/0");
    struct stat stat;
    fstat(f, &stat);
    int file_size = stat.st_size, buffer_size = 0;
    close(f);

    FILE* fd = must_fopen("rom:/0");
    asset_loadf_into(fd, &file_size, NULL, &buffer_size);
    void* asset_buffer = malloc((size_t)buffer_size);
    fseek(fd, 0, SEEK_SET);
    asset_loadf_into(fd, &file_size, asset_buffer, &buffer_size);
    sprite_t* my_sprite = (sprite_t*)asset_buffer;
    fclose(fd);

    (void)my_sprite;

    while (1) {
        surface_t* disp = display_get();
        surface_t* zbuf = display_get_zbuf();
        rdpq_attach(disp, zbuf);

        rdpq_detach_show();
    }
}

#endif