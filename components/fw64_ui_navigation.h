#pragma once

#include <framework64/input.h>

#include <stdint.h>

typedef enum {
    FW64_UI_NAVIGATION_DIRECTION_NONE,
    FW64_UI_NAVIGATION_DIRECTION_UP,
    FW64_UI_NAVIGATION_DIRECTION_DOWN,
    FW64_UI_NAVIGATION_DIRECTION_LEFT,
    FW64_UI_NAVIGATION_DIRECTION_RIGHT
} fw64UiNavigationDirection;

typedef enum {
    FW64_UI_NAVIGATION_FLAGS_NONE        = 0,
    FW64_UI_NAVIGATION_FLAGS_ENABLE_L_R  = 1 << 0
} fw64UiNavigationFlags;

typedef struct {
    fw64Input* input;
    int controller_index;
    Vec2 current_stick;

    fw64UiNavigationDirection nav_direction;
    float nav_time;
    float nav_repeat_time;
    uint16_t nav_count;
    uint16_t did_nav;
    fw64UiNavigationFlags flags;
} fw64UiNavigation;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_ui_navigation_init(fw64UiNavigation* ui, fw64Input* input, int controller_index);
void fw64_ui_navigation_update(fw64UiNavigation* ui, float time_delta);

int fw64_ui_navigation_moved_up(fw64UiNavigation* ui);
int fw64_ui_navigation_moved_down(fw64UiNavigation* ui);
int fw64_ui_navigation_moved_right(fw64UiNavigation* ui);
int fw64_ui_navigation_moved_left(fw64UiNavigation* ui);
int fw64_ui_navigation_accepted(fw64UiNavigation* ui);
int fw64_ui_navigation_back(fw64UiNavigation* ui);

#define fw64_ui_navigation_lr_enabled(ui) ((ui)->flags & FW64_UI_NAVIGATION_FLAGS_ENABLE_L_R)
#define fw64_ui_navigation_enable_lr(ui) (ui)->flags |= FW64_UI_NAVIGATION_FLAGS_ENABLE_L_R
#define fw64_ui_navigation_disable_lr(ui) (ui)->flags &= ~FW64_UI_NAVIGATION_FLAGS_ENABLE_L_R

#ifdef __cplusplus
}
#endif
