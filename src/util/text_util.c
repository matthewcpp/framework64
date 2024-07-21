#include "framework64/util/text_util.h"

IVec2 fw64_text_util_center_string(fw64Font* font, const char* str, const IVec2* size) {
    IVec2 text_measurement = fw64_font_measure_text(font, str);

    IVec2 centered_pos = {
        (size->x / 2) - (text_measurement.x / 2),
        (size->y / 2) - (text_measurement.y / 2)
    };

    return centered_pos;
}
