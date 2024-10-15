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