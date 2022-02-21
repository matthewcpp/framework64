#include "framework64/math.h"

// https://en.wikipedia.org/wiki/Smoothstep
float fw64_smoothstep(float a, float b, float x) {
  // Scale, bias and saturate x to 0..1 range
  x = fw64_clamp((x - a) / (b - a), 0.0, 1.0);
  // Evaluate polynomial
  return x * x * (3.0f - 2.0f * x);
}

float fw64_clamp(float x, float lower, float upper) {
  if (x < lower)
    x = lower;
  if (x > upper)
    x = upper;
  return x;
}

float fw64_maxf(float a, float b) {
  return a >= b ? a : b;
}

float fw64_minf(float a, float b) {
  return a <=b ? a : b;
}