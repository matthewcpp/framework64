#include "framework64/desktop/engine.hpp"

#include "framework64/desktop/asset_database.hpp"
#include "framework64/desktop/audio.hpp"
#include "framework64/desktop/data_link.hpp"
#include "framework64/desktop/filesystem.hpp"
#include "framework64/desktop/input.hpp"
#include "framework64/desktop/media.hpp"
#include "framework64/desktop/renderer.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>

namespace framework64 {
bool Engine::init(Settings const & settings) {
    application_name = settings.application_name;

    if (settings.save_file_type == fw64SaveFile::SaveFileType::Unknown) {
        std::cout << "`Unknown` is not a valid save file type for engine initialization." << std::endl;
        return false;
    }

    const std::string base_path = SDL_GetBasePath();
    const std::string asset_dir_path = base_path + "assets/";
    const std::string shader_dir_path = base_path + "glsl/";
    const std::string save_file_path = base_path + application_name + ".save";
    const std::string media_dir = base_path + settings.media_dir_name;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);

    if (!display.init(settings)) {
        return false;
    }

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

    _fw64_default_allocator_init();

    n64_input_interface = std::make_unique<N64InputInterface>();
    shader_cache = std::make_unique<ShaderCache>(shader_dir_path);

    displays = new fw64Displays(&display);
    renderer = new fw64Renderer(display, *shader_cache);
    save_file = new fw64SaveFile();
    assets = new fw64AssetDatabase(asset_dir_path, *shader_cache);
    audio = new fw64Audio();
    input = new fw64Input();
    media = new fw64Media();
    data_link = new fw64DataLink();

    time = new fw64Time();
    memset(time, 0, sizeof(fw64Time));

    if (!assets->init()) {
        std::cout << "Failed to initialize Asset Database" << std::endl;
        return false;
    }

    if (!renderer->init(settings.screen_width, settings.screen_height)) {
        std::cout << "Failed to initialize renderer" << std::endl;
        return false;
    }

    input->init(*n64_input_interface, *time);
    save_file->init(save_file_path, settings.save_file_type);

    if (!media->init(media_dir)) {
        std::cout << "Failed to initialize media" << std::endl;
        return false;
    }

    if (!data_link->initialize(settings.data_link_port)) {
        std::cout << "Failed to initialize websocket datalink" << std::endl;
        return false;
    }

    Filesystem::init(asset_dir_path, *assets);

    return true;
}

void Engine::update(float time_delta) {
    input->update();
    data_link->update();
    time->time_delta = time_delta;
    time->total_time += time_delta;

    save_file->update(time_delta);
}

}
