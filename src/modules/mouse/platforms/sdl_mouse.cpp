#include "sdl_mouse.h"

#include <memory>

fw64MouseModule* fw64_mouse_module_init() {
    auto module = std::make_unique<fw64MouseModule>();

    module->current_state = SDL_GetMouseState(&module->current_pos.x, &module->current_pos.y);
    
    module->prev_state = module->current_state;
    module->prev_pos = module->current_pos;

    return module.release();
}

void fw64_mouse_module_update(fw64MouseModule* module) {
    module->prev_pos = module->current_pos;
    module->prev_state = module->current_state;

    module->current_state = SDL_GetMouseState(&module->current_pos.x, &module->current_pos.y);
}

int fw64_mouse_button_is_pressed(fw64MouseModule* module, fw64MouseButton button) {
    return static_cast<int>(
        !(module->prev_state & SDL_BUTTON(button)) && 
        (module->current_state & SDL_BUTTON(button))
    );
}
int fw64_mouse_button_is_released(fw64MouseModule* module, fw64MouseButton button) {
    return static_cast<int>(
        (module->prev_state & SDL_BUTTON(button)) && 
        !(module->current_state & SDL_BUTTON(button))
    );

}

int fw64_mouse_button_is_down(fw64MouseModule* module, fw64MouseButton button) {
    return static_cast<int>(module->current_state & SDL_BUTTON(button));
}

IVec2 fw64_mouse_get_position(fw64MouseModule* module) {
    return module->current_pos;
}