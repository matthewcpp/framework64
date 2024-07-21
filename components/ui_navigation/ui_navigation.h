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

typedef struct {
    fw64Input* input;
    int controller_index;
    Vec2 current_stick;

    fw64UiNavigationDirection nav_direction;
    float nav_time;
    float nav_repeat_time;
    uint16_t nav_count;
    uint16_t did_nav;
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

#ifdef __cplusplus
}
#endif
