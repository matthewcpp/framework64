#include "framework64/n64_libultra/compat.h"

float fmaxf(float a, float b) {
    return a > b ? a : b;
}

float fminf(float a, float b) {
    return a < b ? a : b;
}
