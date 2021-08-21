#include "game.h"

#include <SDL2/SDL.h>

int main(int argc, char** argv) {
    fw64Engine engine;
    fw64_engine_init(&engine);

    Game game;
    game_init(&game, &engine);

    SDL_Event event;
    int keep_going = 1;
    Uint32 last_update, now, time_delta;

    // first call to poll event can take some time as systems are initialized.
    // prime it here before tight update loop
    SDL_PollEvent(&event);

    last_update = SDL_GetTicks();

    while (keep_going) {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    keep_going = 0;
                    break;
            }
        }

        now = SDL_GetTicks();
        time_delta = now - last_update;

        if (time_delta >= 32) {
            fw64_engine_update(&engine);
            game_update(&game, engine.time->time_delta);
            game_draw(&game);

            last_update = now;
        }

        SDL_Delay(1);
    }

    return 0;
}
