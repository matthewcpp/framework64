#include "framework64/engine.h"

#include "framework64/desktop/assets.h"
#include "framework64/desktop/audio.h"
#include "framework64/desktop/filesystem.h"
#include "framework64/desktop/input.h"
#include "framework64/desktop/renderer.h"

int fw64_engine_init(fw64Engine* engine) {
    const std::string base_path = SDL_GetBasePath();
    const std::string asset_dir_path = base_path + "assets/";
    const std::string shader_dir_path = base_path + "glsl/";

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);

    engine->assets = new fw64Assets(asset_dir_path);
    engine->audio = new fw64Audio();
    engine->input = new fw64Input();
    engine->renderer = new fw64Renderer();
    engine->time = new fw64Time();

    engine->renderer->init(320, 240, shader_dir_path);
    engine->assets->init();
    fw64_desktop_filesystem_init();

    return 1;
}

void fw64_engine_update(fw64Engine* engine) {

}