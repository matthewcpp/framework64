#include "framework64/rect.h"

int fw64_rect_contains_rect(fw64Rect* a, fw64Rect* b) {

    return  fw64_rect_min_x(b) >= fw64_rect_min_x(a) && fw64_rect_max_x(b) <= fw64_rect_max_x(a) &&
            fw64_rect_min_y(b) >= fw64_rect_min_y(a) && fw64_rect_max_x(b) <= fw64_rect_max_x(a);
}
