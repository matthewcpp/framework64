#include "${game_include_path}"

#include "framework64/desktop/engine.hpp"
#include "framework64/desktop/input.hpp"
#include "framework64/desktop/renderer.hpp"

#include <CLI/CLI.hpp>
#include <SDL2/SDL.h>

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    framework64::Engine engine;
    framework64::Settings settings;

#ifdef FW64_MEDIA_DIR_NAME
    settings.media_dir_name = FW64_MEDIA_DIR_NAME;
#endif

#ifdef FW64_APPLICATION_NAME
    settings.application_name = FW64_APPLICATION_NAME;
#endif

    CLI::App app("framework64");
    CLI11_PARSE(app, argc, argv);

    if (!engine.init(settings)) {
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

            engine.renderer->beginFrame();
            game_draw(&game);
            engine.renderer->endFrame();

            last_update = now;
        }

        SDL_Delay(1);
    }

    return 0;
}

fw64SaveFile::SaveFileType getSaveFileType(std::string const & type_name) {
    if (type_name == "NONE")
        return fw64SaveFile::SaveFileType::None;
    else if (type_name == "N64_EEPROM_4K")
        return fw64SaveFile::SaveFileType::N64Eeprom4k;
    else if (type_name == "N64_EEPROM_16K")
        return fw64SaveFile::SaveFileType::N64Eeprom16K;

    return fw64SaveFile::SaveFileType::Unknown;
}
