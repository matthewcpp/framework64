#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Returns a smooth interpolation between 0 and 1 for the value X (where X ranges between A and B) Clamped to 0 for X <= A and 1 for X >= B. */
float fw64_smoothstep(float a, float b, float x);
float fw64_lerp(float a, float b, float t);
float fw64_clamp(float x, float lower, float upper);

float fw64_maxf(float a, float b);
float fw64_minf(float a, float b);

int fw64_maxi(int a, int b);
int fw64_mini(int a, int b);

float fw64_map_value_in_numeric_range(float value, float input_start, float input_end, float output_start, float output_end);

#ifdef __cplusplus
}
#endif