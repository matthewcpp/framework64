#include "framework64/desktop/engine.h"

#include "framework64/desktop/assets.h"
#include "framework64/desktop/audio.h"
#include "framework64/desktop/filesystem.h"
#include "framework64/desktop/input.h"
#include "framework64/desktop/renderer.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

namespace framework64 {
bool Engine::init(int screen_width, int screen_height) {
    const std::string base_path = SDL_GetBasePath();
    const std::string asset_dir_path = base_path + "assets/";
    const std::string shader_dir_path = base_path + "glsl/";

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);

    int result = IMG_Init(IMG_INIT_PNG);

    if ((result & IMG_INIT_PNG) != IMG_INIT_PNG) {
        return false;
    }

    result = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
    if (result)
        return false;

    result = Mix_Init(MIX_INIT_OGG);

    if ((result & MIX_INIT_OGG) != MIX_INIT_OGG) {
        return false;
    }

    n64_input_interface = std::make_unique<N64InputInterface>();
    shader_cache = std::make_unique<ShaderCache>(shader_dir_path);

    renderer = new fw64Renderer();
    assets = new fw64Assets(asset_dir_path, *shader_cache);
    audio = new fw64Audio();
    input = new fw64Input();

    time = new fw64Time();
    memset(time, 0, sizeof(fw64Time));

    if (!renderer->init(screen_width, screen_height, shader_dir_path))
        return false;

    input->init(*n64_input_interface);

    if (!assets->init())
        return false;

    fw64_desktop_filesystem_init();

    return true;
}

void Engine::update(float time_delta) {
    input->update();
    time->time_delta = time_delta;
    time->total_time += time_delta;
}
}
