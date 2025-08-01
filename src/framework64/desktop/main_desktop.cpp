#include "${game_include_path}"

#include "framework64/desktop/engine.hpp"
#include "framework64/desktop/input.hpp"
#include "framework64/desktop/renderer.hpp"

#include <CLI/CLI.hpp>
#include <SDL2/SDL.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

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

    const uint64_t target_frame_time_ms = 1000 / settings.target_frame_rate;
    const uint64_t fixed_timestep_ms = 1000 / settings.fixed_update_rate;
    uint64_t accumulator = 0;

    auto last_update = std::chrono::steady_clock::now();

    // first call to poll event can take some time as systems are initialized.
    // prime it here before tight update loop
    SDL_PollEvent(&event);

    while (keep_going) {
        auto frame_start = std::chrono::steady_clock::now();

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

        auto frame_delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(frame_start - last_update).count();
        accumulator += frame_delta_ms;

        // do we need an engine fixed update? e.g. physics simulation
        while (accumulator > fixed_timestep_ms) {
            game_fixed_update(&game);
            accumulator -= fixed_timestep_ms;
        }

        float accumulator_progress = static_cast<float>(accumulator) / static_cast<float>(fixed_timestep_ms);
        engine.update(static_cast<float>(frame_delta_ms) / 1000.0f, accumulator_progress);
        game_update(&game);

        // todo: time to next step?
        engine.renderer->beginFrame();
        game_draw(&game);
        engine.renderer->endFrame();


        auto frame_end = std::chrono::steady_clock::now();
        uint64_t frame_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start).count();
        last_update = frame_start;

        if (frame_time_ms < target_frame_time_ms) {
            std::this_thread::sleep_for(std::chrono::milliseconds(target_frame_time_ms - frame_time_ms));
        }
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
