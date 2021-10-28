#include "framework64/n64/compat.h"

float fmaxf(float a, float b) {
    return a > b ? a : b;
}

float fminf(float a, float b) {
    return a < b ? a : b;
}
