#include "ui_navigation.h"

#include "framework64/controller_mapping/n64.h"

#include <string.h>
#include <float.h>

#define STICK_THRESHOLD 0.2f
#define INITIAL_NAV_HOLD_TIME 0.25f
#define DEFAULT_NAV_REPEAT_TIME 0.15f
#define NOT_CUURENTLY_NAVIGATING FLT_MIN

void fw64_ui_navigation_init(fw64UiNavigation* ui, fw64Input* input, int controller_index) {
    memset(ui, 0, sizeof(fw64UiNavigation));

    ui->input = input;
    ui->nav_repeat_time = DEFAULT_NAV_REPEAT_TIME;
    ui->controller_index = controller_index;
    ui->nav_time = NOT_CUURENTLY_NAVIGATING;
    fw64_input_controller_stick(ui->input, ui->controller_index, &ui->current_stick);
}

static int fw64_ui_navigation_down(fw64UiNavigation* ui) {
    return (ui->current_stick.y < -STICK_THRESHOLD ||
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_C_DOWN) || 
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN));
}

static int fw64_ui_navigation_up(fw64UiNavigation* ui) {
    return (ui->current_stick.y > STICK_THRESHOLD ||
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_C_UP) || 
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_DPAD_UP));
}

static int fw64_ui_navigation_right(fw64UiNavigation* ui) {
    return (ui->current_stick.x > STICK_THRESHOLD ||
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT) || 
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT) ||
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_R));
}

static int fw64_ui_navigation_left(fw64UiNavigation* ui) {
    return (ui->current_stick.x < -STICK_THRESHOLD ||
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_C_LEFT) || 
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT) ||
        fw64_input_controller_button_down(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_L));
}

static fw64UiNavigationDirection fw64_ui_navigation_check_if_nav_started(fw64UiNavigation* ui) {
    if (fw64_ui_navigation_up(ui)) {
        return FW64_UI_NAVIGATION_DIRECTION_UP;
    } else if (fw64_ui_navigation_down(ui)) {
        return FW64_UI_NAVIGATION_DIRECTION_DOWN;
    } else if (fw64_ui_navigation_left(ui)) {
        return FW64_UI_NAVIGATION_DIRECTION_LEFT;
    } else if (fw64_ui_navigation_right(ui)) {
        return FW64_UI_NAVIGATION_DIRECTION_RIGHT;
    }

    return FW64_UI_NAVIGATION_DIRECTION_NONE;
}

static int fw64_ui_navigation_is_still_navigating(fw64UiNavigation* ui) {
    switch(ui->nav_direction) {
        case FW64_UI_NAVIGATION_DIRECTION_NONE:
            return 0;

        case FW64_UI_NAVIGATION_DIRECTION_UP:
            return fw64_ui_navigation_up(ui);

        case FW64_UI_NAVIGATION_DIRECTION_DOWN:
            return fw64_ui_navigation_down(ui);

        case FW64_UI_NAVIGATION_DIRECTION_LEFT:
            return fw64_ui_navigation_left(ui);

        case FW64_UI_NAVIGATION_DIRECTION_RIGHT:
            return fw64_ui_navigation_right(ui);
    }

    return 0;
}

void fw64_ui_navigation_update(fw64UiNavigation* ui, float time_delta) {
    fw64_input_controller_stick(ui->input, ui->controller_index, &ui->current_stick);

    if (ui->nav_time == NOT_CUURENTLY_NAVIGATING) {
        ui->nav_direction = fw64_ui_navigation_check_if_nav_started(ui);

        if (ui->nav_direction != FW64_UI_NAVIGATION_DIRECTION_NONE) {
            ui->nav_time = 0.0f;
            ui->nav_count = 0;
            ui->did_nav = 0;
        }
    } else {
        if (!fw64_ui_navigation_is_still_navigating(ui)) {
            ui->nav_time = NOT_CUURENTLY_NAVIGATING;

            if (ui->nav_count == 0) {
                ui->did_nav = 1;
            }
        }
        else {
            ui->nav_time += time_delta;
            float wait_time = ui->nav_count == 0 ? INITIAL_NAV_HOLD_TIME : ui->nav_repeat_time;

            if (ui->nav_time >= wait_time) {
                ui->nav_count += 1;
                ui->did_nav = 1;
                ui->nav_time -= wait_time;
            } else {
                ui->did_nav = 0;
            }
        }
    }

}

int fw64_ui_navigation_moved_up(fw64UiNavigation* ui) {
    return ui->nav_direction == FW64_UI_NAVIGATION_DIRECTION_UP && ui->did_nav;
}

int fw64_ui_navigation_moved_down(fw64UiNavigation* ui) {
    return ui->nav_direction == FW64_UI_NAVIGATION_DIRECTION_DOWN && ui->did_nav;
}

int fw64_ui_navigation_moved_right(fw64UiNavigation* ui) {
    return ui->nav_direction == FW64_UI_NAVIGATION_DIRECTION_RIGHT && ui->did_nav;
}

int fw64_ui_navigation_moved_left(fw64UiNavigation* ui) {
    return ui->nav_direction == FW64_UI_NAVIGATION_DIRECTION_LEFT && ui->did_nav;
}

int fw64_ui_navigation_accepted(fw64UiNavigation* ui) {
    return (fw64_input_controller_button_released(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_A) || 
            fw64_input_controller_button_released(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_START));
}

int fw64_ui_navigation_back(fw64UiNavigation* ui) {
    return (fw64_input_controller_button_released(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_B) || 
            fw64_input_controller_button_released(ui->input, ui->controller_index, FW64_N64_CONTROLLER_BUTTON_Z));
}
