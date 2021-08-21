#include "framework64/engine.h"

#include "framework64/desktop/assets.h"
#include "framework64/desktop/audio.h"
#include "framework64/desktop/filesystem.h"
#include "framework64/desktop/input.h"
#include "framework64/desktop/renderer.h"

int fw64_engine_init(fw64Engine* engine) {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);

    engine->assets = new fw64Assets();
    engine->audio = new fw64Audio();
    engine->input = new fw64Input();
    engine->renderer = new fw64Renderer();
    engine->time = new fw64Time();

    const std::string base_dir = "/Users/matthew/development/repos/n64-sandbox/src/desktop/";
    fw64_desktop_renderer_init(engine->renderer, 320, 240, base_dir);
    fw64_desktop_filesystem_init();

    return 1;
}

void fw64_engine_update(fw64Engine* engine) {

}