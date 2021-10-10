#include "game.h"
#include "framework64/desktop/engine.h"
#include "framework64/desktop/input.h"

#include <SDL2/SDL.h>

#include <iostream>

int main(int argc, char** argv) {
    framework64::Engine engine;
    if (!engine.init(320, 240)) {
        std::cout << "Failed to initialize engine." << std::endl;
        return 1;
    }

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

                case SDL_JOYDEVICEADDED:
                    engine.input->onDeviceAdded(event.jdevice.which);
                    break;

                case SDL_JOYDEVICEREMOVED:
                    engine.input->onDeviceRemoved(event.jdevice.which);
                    break;
            }
        }

        now = SDL_GetTicks();
        time_delta = now - last_update;

        if (time_delta >= 32) {
            engine.update(static_cast<float>(time_delta) / 1000.0f);
            game_update(&game);
            game_draw(&game);

            last_update = now;
        }

        SDL_Delay(1);
    }

    return 0;
}
