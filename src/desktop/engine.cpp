#include "framework64/desktop/engine.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/audio.h"
#include "framework64/desktop/filesystem.h"
#include "framework64/desktop/input.h"
#include "framework64/desktop/renderer.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>

namespace framework64 {
bool Engine::init(int screen_width, int screen_height) {
    const std::string base_path = SDL_GetBasePath();
    const std::string asset_dir_path = base_path + "assets/";
    const std::string database_path = asset_dir_path + "assets.db";
    const std::string shader_dir_path = base_path + "glsl/";

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);

    int result = IMG_Init(IMG_INIT_PNG);

    if ((result & IMG_INIT_PNG) != IMG_INIT_PNG) {
        std::cout << "Failed to initialize SDL_Image" << std::endl;
        return false;
    }

    result = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
    if (result) {
        std::cout << "Failed to initialize audio output" << std::endl;
        return false;
    }


    result = Mix_Init(MIX_INIT_OGG);

    if ((result & MIX_INIT_OGG) != MIX_INIT_OGG) {
        std::cout << "SDL_Mixer: Failed to initialize OGG Library" << std::endl;
        return false;
    }

    fw64_default_allocator_init();

    n64_input_interface = std::make_unique<N64InputInterface>();
    shader_cache = std::make_unique<ShaderCache>(shader_dir_path);

    renderer = new fw64Renderer();
    assets = new fw64AssetDatabase(asset_dir_path, *shader_cache);
    audio = new fw64Audio();
    input = new fw64Input();

    time = new fw64Time();
    memset(time, 0, sizeof(fw64Time));

    if (!assets->init(database_path)) {
        std::cout << "Failed to initialize Asset Database" << std::endl;
        return false;
    }

    if (!renderer->init(screen_width, screen_height, shader_dir_path)) {
        std::cout << "Failed to initialize renderer" << std::endl;
        return false;
    }

    input->init(*n64_input_interface);

    Filesystem::init(asset_dir_path, *assets);

    return true;
}

void Engine::update(float time_delta) {
    input->update();
    time->time_delta = time_delta;
    time->total_time += time_delta;
}
}
