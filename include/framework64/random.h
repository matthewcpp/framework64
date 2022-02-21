#pragma once

/** \file random.h */

#include <stdint.h>

#define FW64_RANDOM_MAX 32767

#ifdef __cplusplus
extern "C" {
#endif

/** Seeds the random number generator. Initial Seed value is 1. */
void fw64_random_set_seed(uint32_t seed_val);

/** Generates a random integer i the range [0, FW64_RANDOM_MAX] */
int fw64_random_int();

/** Generates a random integer i the range [min, max] */
int fw64_random_int_in_range(int min, int max);

/** Generates a random float in the range [0.0, 1.0] */
float fw64_random_float();

/** Generates a random float in the range [min_val, max_val] */
float fw64_random_float_in_range(float min_val, float max_val);

#ifdef __cplusplus
}
#endif