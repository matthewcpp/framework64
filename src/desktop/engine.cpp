#include "framework64/desktop/engine.h"

#include "framework64/desktop/assets.h"
#include "framework64/desktop/audio.h"
#include "framework64/desktop/filesystem.h"
#include "framework64/desktop/input.h"
#include "framework64/desktop/renderer.h"

namespace framework64 {
bool Engine::init(int screen_width, int screen_height) {
    const std::string base_path = SDL_GetBasePath();
    const std::string asset_dir_path = base_path + "assets/";
    const std::string shader_dir_path = base_path + "glsl/";

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);

    renderer = new fw64Renderer();
    assets = new fw64Assets(asset_dir_path, renderer->mesh_renderer);
    audio = new fw64Audio();
    input = new fw64Input();

    time = new fw64Time();
    memset(time, 0, sizeof(fw64Time));

    if (!renderer->init(screen_width, screen_height, shader_dir_path))
        return false;

    assets->init();
    fw64_desktop_filesystem_init();

    return true;
}

void Engine::update(float time_delta) {
    time->time_delta = time_delta;
    time->total_time += time_delta;
}
}
