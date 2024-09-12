#include "framework64/random.h"

static uint32_t random_seed = 1;

void fw64_random_set_seed(uint32_t seed_val) {
    random_seed = seed_val;
}

int fw64_random_int() {
    random_seed = random_seed * 1103515245 + 12345;
    return (unsigned int)(random_seed/65536) % 32768;
}

int fw64_random_int_in_range(int min_val, int max_val) {
    return min_val + (fw64_random_int() % (max_val - min_val + 1));
}

float fw64_random_float() {
    return (float)fw64_random_int() / (float)FW64_RANDOM_MAX;
}

float fw64_random_float_in_range(float min_val, float max_val) {
    return min_val + (fw64_random_float() * (max_val - min_val));
}
