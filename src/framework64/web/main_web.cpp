#include "${game_include_path}"

#include "framework64/desktop/engine.hpp"
#include "framework64/desktop/input.hpp"
#include "framework64/desktop/renderer.hpp"

#include <SDL2/SDL.h>

#include <emscripten.h>

#include <iostream>
#include <chrono>

void tick();

uint64_t target_frame_time_ms;
uint64_t fixed_timestep_ms;
uint64_t accumulator = 0;
auto last_update = std::chrono::steady_clock::now();

framework64::Engine engine;
Game game;

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    framework64::Settings settings;

#ifdef FW64_DESKTOP_CONTROLLER0_EMULATION
    settings.controller0KeyoardEmulation = FW64_DESKTOP_CONTROLLER0_EMULATION;
#endif

    settings.application_name = FW64_APPLICATION_NAME;
    target_frame_time_ms = 1000 / settings.target_frame_rate;
    fixed_timestep_ms = 1000 / settings.fixed_update_rate;

    if (!engine.init(settings)) {
        std::cout << "Failed to initialize engine." << std::endl;
        return 1;
    }

    game_init(&game, &engine);

    emscripten_set_main_loop(tick, 0, true);

    return 0;
}

void tick()
{
    auto frame_start = std::chrono::steady_clock::now();

    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            emscripten_cancel_main_loop();
            return;

        case SDL_JOYDEVICEADDED:
            engine.input->onDeviceAdded(event.jdevice.which);
            break;

        case SDL_JOYDEVICEREMOVED:
            engine.input->onDeviceRemoved(event.jdevice.which);
            break;
    }

    auto frame_delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(frame_start - last_update).count();
    accumulator += frame_delta_ms;

    while (accumulator > fixed_timestep_ms) {
        game_fixed_update(&game);
        accumulator -= fixed_timestep_ms;
    }

    float accumulator_progress = static_cast<float>(accumulator) / static_cast<float>(fixed_timestep_ms);
    engine.update(static_cast<float>(frame_delta_ms) / 1000.0f, accumulator_progress);
    game_update(&game);

    engine.renderer->beginFrame();
    game_draw(&game);
    engine.renderer->endFrame();

    last_update = frame_start;

    return;
}